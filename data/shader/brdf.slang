import math_constants;

//------------------------------------------------------------------------------
// BRDF configuration
//------------------------------------------------------------------------------

// Diffuse BRDFs
#define DIFFUSE_LAMBERT             0
#define DIFFUSE_BURLEY              1

// Specular BRDF
// Normal distribution functions
#define SPECULAR_D_GGX              0

// Anisotropic NDFs
#define SPECULAR_D_GGX_ANISOTROPIC  0

// Cloth NDFs
#define SPECULAR_D_CHARLIE          0

// Visibility functions
#define SPECULAR_V_SMITH_GGX        0
#define SPECULAR_V_SMITH_GGX_FAST   1
#define SPECULAR_V_GGX_ANISOTROPIC  2
#define SPECULAR_V_KELEMEN          3
#define SPECULAR_V_NEUBELT          4

// Fresnel functions
#define SPECULAR_F_SCHLICK          0

#define BRDF_DIFFUSE                DIFFUSE_LAMBERT

#if FILAMENT_QUALITY < FILAMENT_QUALITY_HIGH
#define BRDF_SPECULAR_D             SPECULAR_D_GGX
#define BRDF_SPECULAR_V             SPECULAR_V_SMITH_GGX_FAST
#define BRDF_SPECULAR_F             SPECULAR_F_SCHLICK
#else
#define BRDF_SPECULAR_D             SPECULAR_D_GGX
#define BRDF_SPECULAR_V             SPECULAR_V_SMITH_GGX
#define BRDF_SPECULAR_F             SPECULAR_F_SCHLICK
#endif

#define BRDF_CLEAR_COAT_D           SPECULAR_D_GGX
#define BRDF_CLEAR_COAT_V           SPECULAR_V_KELEMEN

#define BRDF_ANISOTROPIC_D          SPECULAR_D_GGX_ANISOTROPIC
#define BRDF_ANISOTROPIC_V          SPECULAR_V_GGX_ANISOTROPIC

#define BRDF_CLOTH_D                SPECULAR_D_CHARLIE
#define BRDF_CLOTH_V                SPECULAR_V_NEUBELT

//------------------------------------------------------------------------------
// Specular BRDF implementations
//------------------------------------------------------------------------------

float32_t D_GGX(float32_t roughness, float32_t NoH, const float32_t3 h) {
    // Walter et al. 2007, "Microfacet Models for Refraction through Rough Surfaces"

    // In mediump, there are two problems computing 1.0 - NoH^2
    // 1) 1.0 - NoH^2 suffers floating point cancellation when NoH^2 is close to 1 (highlights)
    // 2) NoH doesn't have enough precision around 1.0
    // Both problem can be fixed by computing 1-NoH^2 in  and providing NoH in  as well

    // However, we can do better using Lagrange's identity:
    //      ||a x b||^2 = ||a||^2 ||b||^2 - (a . b)^2
    // since N and H are unit vectors: ||N x H||^2 = 1.0 - NoH^2
    // This computes 1.0 - NoH^2 directly (which is close to zero in the highlights and has
    // enough precision).
    // Overall this yields better performance, keeping all computations in mediump

    float32_t oneMinusNoHSquared = 1.0 - NoH * NoH;

    float32_t a = NoH * roughness;
    float32_t k = min(roughness / (oneMinusNoHSquared + a * a), 453.5); // 453.5 prevents fp16 overflow
    float32_t d = k * (k * (1.0 / math_st::pi<float32_t>()));
    //let ppp = math_st::pi<float32_t>()
    return d;
}

float32_t D_GGX_Anisotropic(float32_t at, float32_t ab, float32_t ToH, float32_t BoH, float32_t NoH) {
    // Burley 2012, "Physically-Based Shading at Disney"

    // The values at and ab are perceptualRoughness^2, a2 is therefore perceptualRoughness^4
    // The dot product below computes perceptualRoughness^8. We cannot fit in fp16 without clamping
    // the roughness to too high values so we perform the dot product and the division in fp32
    float32_t a2 = at * ab;
     float32_t3 d = float32_t3(ab * ToH, at * BoH, a2 * NoH);
     float32_t d2 = dot(d, d);
    float32_t b2 = a2 / d2;
    return a2 * b2 * b2 * (1.0 / math_st::pi<float32_t>());
}

float32_t D_Charlie(float32_t roughness, float32_t NoH) {
    // Estevez and Kulla 2017, "Production Friendly Microfacet Sheen BRDF"
    float32_t invAlpha  = 1.0 / roughness;
    float32_t cos2h = NoH * NoH;
    float32_t sin2h = max(1.0 - cos2h, 0.0078125); // 2^(-14/2), so sin2h^2 > 0 in fp16
    return (2.0 + invAlpha) * pow(sin2h, invAlpha * 0.5) / (2.0 * math_st::pi<float32_t>());
}

float32_t V_SmithGGXCorrelated(float32_t roughness, float32_t NoV, float32_t NoL) {
    // Heitz 2014, "Understanding the Masking-Shadowing Function in Microfacet-Based BRDFs"
    float32_t a2 = roughness * roughness;
    // TODO: lambdaV can be pre-computed for all the lights, it should be moved out of this function
    float32_t lambdaV = NoL * sqrt((NoV - a2 * NoV) * NoV + a2);
    float32_t lambdaL = NoV * sqrt((NoL - a2 * NoL) * NoL + a2);
    // 0.0000077 = nextafter(0.5 / MEDIUMP_FLT_MAX, 1.0) in fp16, so we don't overflow
    float32_t v = 0.5 / lambdaV + lambdaL;
    // a2=0 => v = 1 / 4*NoL*NoV   => min=1/4, max=+inf
    // a2=1 => v = 1 / 2*(NoL+NoV) => min=1/4, max=+inf
    return v;
}

float32_t V_SmithGGXCorrelated_Fast(float32_t roughness, float32_t NoV, float32_t NoL) {
    // Hammon 2017, "PBR Diffuse Lighting for GGX+Smith Microsurfaces"
    // 0.0000077 = nextafter(0.5 / MEDIUMP_FLT_MAX, 1.0) in fp16, so we don't overflow
    float32_t v = 0.5 / lerp(2.0 * NoL * NoV, NoL + NoV, roughness);
    return v;
}

float32_t V_SmithGGXCorrelated_Anisotropic(float32_t at, float32_t ab, float32_t ToV, float32_t BoV,
        float32_t ToL, float32_t BoL, float32_t NoV, float32_t NoL) {
    // Heitz 2014, "Understanding the Masking-Shadowing Function in Microfacet-Based BRDFs"
    // TODO: lambdaV can be pre-computed for all the lights, it should be moved out of this function
    float32_t lambdaV = NoL * length(float32_t3(at * ToV, ab * BoV, NoV));
    float32_t lambdaL = NoV * length(float32_t3(at * ToL, ab * BoL, NoL));
    // 0.0000077 = nextafter(0.5 / MEDIUMP_FLT_MAX, 1.0) in fp16, so we don't overflow
    float32_t v = 0.5 / lambdaV + lambdaL;
    return v;
}

float32_t V_Kelemen(float32_t LoH) {
    // Kelemen 2001, "A Microfacet Based Coupled Specular-Matte BRDF Model with Importance Sampling"
    // 0.0000039 = nextafter(0.25 / MEDIUMP_FLT_MAX, 1.0) in fp16, so we don't overflow
    return 0.25 / LoH * LoH;
}

float32_t V_Neubelt(float32_t NoV, float32_t NoL) {
    // Neubelt and Pettineo 2013, "Crafting a Next-gen Material Pipeline for The Order: 1886"
    // 0.00001532 = nextafter(1.0 / MEDIUMP_FLT_MAX, 1.0) in fp16, so we don't overflow
    return 1.0 / 4.0 * (NoL + NoV - NoL * NoV);
}

float32_t3 F_Schlick(const float32_t3 f0, float32_t f90, float32_t VoH) {
    // Schlick 1994, "An Inexpensive BRDF Model for Physically-Based Rendering"
    return f0 + (f90 - f0) * pow(1.0 - VoH, 5);
}

float32_t3 F_Schlick(const float32_t3 f0, float32_t VoH) {
    float32_t f = pow(1.0 - VoH, 5.0);
    return f + f0 * (1.0 - f);
}

float32_t F_Schlick(float32_t f0, float32_t f90, float32_t VoH) {
    return f0 + (f90 - f0) * pow(1.0 - VoH, 5);
}

//------------------------------------------------------------------------------
// Specular BRDF dispatch
//------------------------------------------------------------------------------

float32_t distribution(float32_t roughness, float32_t NoH, const float32_t3 h) {
#if BRDF_SPECULAR_D == SPECULAR_D_GGX
    return D_GGX(roughness, NoH, h);
#endif
}

float32_t visibility(float32_t roughness, float32_t NoV, float32_t NoL) {
#if BRDF_SPECULAR_V == SPECULAR_V_SMITH_GGX
    return V_SmithGGXCorrelated(roughness, NoV, NoL);
#elif BRDF_SPECULAR_V == SPECULAR_V_SMITH_GGX_FAST
    return V_SmithGGXCorrelated_Fast(roughness, NoV, NoL);
#endif
}

float32_t3 fresnel(const float32_t3 f0, float32_t LoH) {
#if BRDF_SPECULAR_F == SPECULAR_F_SCHLICK
#if FILAMENT_QUALITY == FILAMENT_QUALITY_LOW
    return F_Schlick(f0, LoH); // f90 = 1.0
#else
    float32_t f90 = saturate(dot(f0, float32_t3(50.0 * 0.33)));
    return F_Schlick(f0, f90, LoH);
#endif
#endif
}

float32_t3 fresnel(const float32_t3 f0, const float32_t f90, float32_t LoH) {
#if BRDF_SPECULAR_F == SPECULAR_F_SCHLICK
    return F_Schlick(f0, f90, LoH);
#endif
}

float32_t distributionAnisotropic(float32_t at, float32_t ab, float32_t ToH, float32_t BoH, float32_t NoH) {
#if BRDF_ANISOTROPIC_D == SPECULAR_D_GGX_ANISOTROPIC
    return D_GGX_Anisotropic(at, ab, ToH, BoH, NoH);
#endif
}

float32_t visibilityAnisotropic(float32_t roughness, float32_t at, float32_t ab,
        float32_t ToV, float32_t BoV, float32_t ToL, float32_t BoL, float32_t NoV, float32_t NoL) {
#if BRDF_ANISOTROPIC_V == SPECULAR_V_SMITH_GGX
    return V_SmithGGXCorrelated(roughness, NoV, NoL);
#elif BRDF_ANISOTROPIC_V == SPECULAR_V_GGX_ANISOTROPIC
    return V_SmithGGXCorrelated_Anisotropic(at, ab, ToV, BoV, ToL, BoL, NoV, NoL);
#endif
}

float32_t distributionClearCoat(float32_t roughness, float32_t NoH, const float32_t3 h) {
#if BRDF_CLEAR_COAT_D == SPECULAR_D_GGX
    return D_GGX(roughness, NoH, h);
#endif
}

float32_t visibilityClearCoat(float32_t LoH) {
#if BRDF_CLEAR_COAT_V == SPECULAR_V_KELEMEN
    return V_Kelemen(LoH);
#endif
}

float32_t distributionCloth(float32_t roughness, float32_t NoH) {
#if BRDF_CLOTH_D == SPECULAR_D_CHARLIE
    return D_Charlie(roughness, NoH);
#endif
}

float32_t visibilityCloth(float32_t NoV, float32_t NoL) {
#if BRDF_CLOTH_V == SPECULAR_V_NEUBELT
    return V_Neubelt(NoV, NoL);
#endif
}

//------------------------------------------------------------------------------
// Diffuse BRDF implementations
//------------------------------------------------------------------------------

float32_t Fd_Lambert() {
    return 1.0 / math_st::pi<float32_t>();
}

float32_t Fd_Burley(float32_t roughness, float32_t NoV, float32_t NoL, float32_t LoH) {
    // Burley 2012, "Physically-Based Shading at Disney"
    float32_t f90 = 0.5 + 2.0 * roughness * LoH * LoH;
    float32_t lightScatter = F_Schlick(1.0, f90, NoL);
    float32_t viewScatter  = F_Schlick(1.0, f90, NoV);
    return lightScatter * viewScatter * (1.0 / math_st::pi<float32_t>());
}

// Energy conserving wrap diffuse term, does *not* include the divide by math_st::pi<float32_t>()
float32_t Fd_Wrap(float32_t NoL, float32_t w) {
    return saturate((NoL + w) / pow(1.0 + w, 2));
}

//------------------------------------------------------------------------------
// Diffuse BRDF dispatch
//------------------------------------------------------------------------------

float32_t diffuse(float32_t roughness, float32_t NoV, float32_t NoL, float32_t LoH) {
#if BRDF_DIFFUSE == DIFFUSE_LAMBERT
    return Fd_Lambert();
#elif BRDF_DIFFUSE == DIFFUSE_BURLEY
    return Fd_Burley(roughness, NoV, NoL, LoH);
#endif
}

//------------------------------------------------------------------------------
// surface shading model standard
//------------------------------------------------------------------------------

// fragment
struct PixelParams {
    float32_t3  diffuseColor;
    float perceptualRoughness;
    float perceptualRoughnessUnclamped;
    float32_t3  f0;
#if defined(MATERIAL_HAS_SPECULAR_COLOR_FACTOR) || defined(MATERIAL_HAS_SPECULAR_FACTOR)
    float f90;
    float specular;
    float32_t3  specularColor;
#endif
    float roughness;
    float32_t3  dfg;
    float32_t3  energyCompensation;

#if defined(MATERIAL_HAS_CLEAR_COAT)
    float clearCoat;
    float clearCoatPerceptualRoughness;
    float clearCoatRoughness;
#endif

#if defined(MATERIAL_HAS_SHEEN_COLOR)
    float32_t3  sheenColor;
#if !defined(SHADING_MODEL_CLOTH)
    float sheenRoughness;
    float sheenPerceptualRoughness;
    float sheenScaling;
    float sheenDFG;
#endif
#endif

#if defined(MATERIAL_HAS_ANISOTROPY)
    float32_t3  anisotropicT;
    float32_t3  anisotropicB;
    float anisotropy;
#endif

#if defined(SHADING_MODEL_SUBSURFACE) || defined(MATERIAL_HAS_REFRACTION)
    float thickness;
#endif
#if defined(SHADING_MODEL_SUBSURFACE)
    float32_t3  subsurfaceColor;
    float subsurfacePower;
#endif

#if defined(SHADING_MODEL_CLOTH) && defined(MATERIAL_HAS_SUBSURFACE_COLOR)
    float32_t3  subsurfaceColor;
#endif

#if defined(MATERIAL_HAS_REFRACTION)
    float etaRI;
    float etaIR;
    float transmission;
    float uThickness;
    float32_t3  absorption;
#endif
};

// light
struct Light {
  float32_t4 colorIntensity;  // rgb, pre-exposed intensity
  float32_t3 l;
  float attenuation;
  float32_t3 worldPosition;
  float NoL;
  float32_t3 direction;
  float zLight;
  bool castsShadows;
  bool contactShadows;
  uint lightType;
  int shadowIndex;
  int channels;
};

// ============

#if defined(MATERIAL_HAS_SHEEN_COLOR)
float32_t3 sheenLobe(const PixelParams pixel, float NoV, float NoL, float NoH) {
    float D = distributionCloth(pixel.sheenRoughness, NoH);
    float V = visibilityCloth(NoV, NoL);

    return (D * V) * pixel.sheenColor;
}
#endif

#if defined(MATERIAL_HAS_CLEAR_COAT)
float clearCoatLobe(const PixelParams pixel, const float32_t3 h, float NoH, float LoH, out float Fcc) {
#if defined(MATERIAL_HAS_NORMAL) || defined(MATERIAL_HAS_CLEAR_COAT_NORMAL)
    // If the material has a normal map, we want to use the geometric normal
    // instead to avoid applying the normal map details to the clear coat layer
    float clearCoatNoH = saturate(dot(shading_clearCoatNormal, h));
#else
    float clearCoatNoH = NoH;
#endif

    // clear coat specular lobe
    float D = distributionClearCoat(pixel.clearCoatRoughness, clearCoatNoH, h);
    float V = visibilityClearCoat(LoH);
    float F = F_Schlick(0.04, 1.0, LoH) * pixel.clearCoat; // fix IOR to 1.5

    Fcc = F;
    return D * V * F;
}
#endif

#if defined(MATERIAL_HAS_ANISOTROPY)
float32_t3 anisotropicLobe(const PixelParams pixel, const Light light, const float32_t3 h,
        float NoV, float NoL, float NoH, float LoH) {

    float32_t3 l = light.l;
    float32_t3 t = pixel.anisotropicT;
    float32_t3 b = pixel.anisotropicB;
    float32_t3 v = shading_view;

    float ToV = dot(t, v);
    float BoV = dot(b, v);
    float ToL = dot(t, l);
    float BoL = dot(b, l);
    float ToH = dot(t, h);
    float BoH = dot(b, h);

    // Anisotropic parameters: at and ab are the roughness along the tangent and bitangent
    // to simplify materials, we derive them from a single roughness parameter
    // Kulla 2017, "Revisiting Physically Based Shading at Imageworks"
    float at = max(pixel.roughness * (1.0 + pixel.anisotropy), MIN_ROUGHNESS);
    float ab = max(pixel.roughness * (1.0 - pixel.anisotropy), MIN_ROUGHNESS);

    // specular anisotropic BRDF
    float D = distributionAnisotropic(at, ab, ToH, BoH, NoH);
    float V = visibilityAnisotropic(pixel.roughness, at, ab, ToV, BoV, ToL, BoL, NoV, NoL);
    float32_t3  F = fresnel(pixel.f0, LoH);

    return (D * V) * F;
}
#endif

float32_t3 isotropicLobe(const PixelParams pixel, const Light light, const float32_t3 h,
        float NoV, float NoL, float NoH, float LoH) {

    float D = distribution(pixel.roughness, NoH, h);
    float V = visibility(pixel.roughness, NoV, NoL);
#if defined(MATERIAL_HAS_SPECULAR_COLOR_FACTOR) || defined(MATERIAL_HAS_SPECULAR_FACTOR)
    float32_t3  F = fresnel(pixel.f0, pixel.f90, LoH);
#else
    float32_t3  F = fresnel(pixel.f0, LoH);
#endif
    return (D * V) * F;
    //return D * F;
    //return D * F;
}

float32_t3 specularLobe(const PixelParams pixel, const Light light, const float32_t3 h,
        float NoV, float NoL, float NoH, float LoH) {
#if defined(MATERIAL_HAS_ANISOTROPY)
    return anisotropicLobe(pixel, light, h, NoV, NoL, NoH, LoH);
#else
    return isotropicLobe(pixel, light, h, NoV, NoL, NoH, LoH);
#endif
}

float32_t3 diffuseLobe(const PixelParams pixel, float NoV, float NoL, float LoH) {
    return pixel.diffuseColor * diffuse(pixel.roughness, NoV, NoL, LoH);
}

/**
 * Evaluates lit materials with the standard shading model. This model comprises
 * of 2 BRDFs: an optional clear coat BRDF, and a regular surface BRDF.
 *
 * Surface BRDF
 * The surface BRDF uses a diffuse lobe and a specular lobe to render both
 * dielectrics and conductors. The specular lobe is based on the Cook-Torrance
 * micro-facet model (see surface_brdf.fs for more details). In addition, the specular
 * can be either isotropic or anisotropic.
 *
 * Clear coat BRDF
 * The clear coat BRDF simulates a transparent, absorbing dielectric layer on
 * top of the surface. Its IOR is set to 1.5 (polyutherane) to simplify
 * our computations. This BRDF only contains a specular lobe and while based
 * on the Cook-Torrance microfacet model, it uses cheaper terms than the surface
 * BRDF's specular lobe (see surface_brdf.fs).
 */
 /*
float32_t3 surfaceShading(const PixelParams pixel, const Light light, float occlusion) {
    float32_t3 h = normalize(shading_view + light.l);

    float NoV = shading_NoV;
    float NoL = saturate(light.NoL);
    float NoH = saturate(dot(shading_normal, h));
    float LoH = saturate(dot(light.l, h));

    float32_t3 Fr = specularLobe(pixel, light, h, NoV, NoL, NoH, LoH);
    float32_t3 Fd = diffuseLobe(pixel, NoV, NoL, LoH);
#if defined(MATERIAL_HAS_REFRACTION)
    Fd *= (1.0 - pixel.transmission);
#endif

    // TODO: attenuate the diffuse lobe to avoid energy gain

    // The energy compensation term is used to counteract the darkening effect
    // at high roughness
    float32_t3 color = Fd + Fr * pixel.energyCompensation;

#if defined(MATERIAL_HAS_SHEEN_COLOR)
    color *= pixel.sheenScaling;
    color += sheenLobe(pixel, NoV, NoL, NoH);
#endif

#if defined(MATERIAL_HAS_CLEAR_COAT)
    float Fcc;
    float clearCoat = clearCoatLobe(pixel, h, NoH, LoH, Fcc);
    float attenuation = 1.0 - Fcc;

#if defined(MATERIAL_HAS_NORMAL) || defined(MATERIAL_HAS_CLEAR_COAT_NORMAL)
    color *= attenuation * NoL;

    // If the material has a normal map, we want to use the geometric normal
    // instead to avoid applying the normal map details to the clear coat layer
    float clearCoatNoL = saturate(dot(shading_clearCoatNormal, light.l));
    color += clearCoat * clearCoatNoL;

    // Early exit to avoid the extra multiplication by NoL
    return (color * light.colorIntensity.rgb) *
            (light.colorIntensity.w * light.attenuation * occlusion);
#else
    color *= attenuation;
    color += clearCoat;
#endif
#endif
    return (color * light.colorIntensity.rgb) *
            (light.colorIntensity.w * light.attenuation * NoL * occlusion);
}*/