import brdf;

struct input_vertex_st {
  float32_t3 m_position;
  float32_t3 m_normal;
  float32_t4 m_tangent;
  float32_t2 m_uv;
};

struct push_constants_st {
  struct view_data_st {
    float32_t4 m_view_position;
    float32_t4 m_view_direction;
  };
  struct projection_data_st {
    struct extent_st {
      uint32_t m_width;
      uint32_t m_height;
    };

    extent_st m_window_extent;
    float32_t m_aspect_ratio;
    float32_t m_near_clip;
    float32_t m_far_clip;
  };
  struct buffer_device_address_registers_st {
    void* m_registers[8];
  };
  struct material_data_st {
    typedef uint16_t descriptor_index_t;

    descriptor_index_t m_base_color_map;
    descriptor_index_t m_metallic_roughness_map;
    descriptor_index_t m_normal_map;
  };

  float32_t4x4 m_model_transformation;
  view_data_st m_view_data;
  projection_data_st m_projection_data;
  buffer_device_address_registers_st m_registers;
  material_data_st m_material_data;

  float64_t m_current_time;
  float64_t m_delta_time;
};

[vk::push_constant]
uniform push_constants_st push_constants;
uniform Sampler2D combined_image_samplers[3];


static const float4x4 proj = {0.5627, 0, 0, 0, 0, 1, 0, 0, 0, 0, -1.0002, -1, 0, 0, -0.2000, 0};
static const float4x4 proj2 = {0.5625, 0, 0, 0, 0, 1, 0, 0, 0, 0, -1.0000, -1, 0, 0, -0.0200, 0};
static const float4x4 proj3 = {0.5625, 0, 0, 0, 0, 1, 0, 0, 0, 0, -1.0101, -1, 0, 0, -0.2010, 0};
static const float4x4 identity = {1.0, 0.0, 0.0, 0.0,
                                  0.0, 1.0, 0.0, 0.0,
                                  0.0, 0.0, 1.0, 0.0,
                                  0.0, 0.0, 0.0, 1.0};
struct output_vertex_st {
  float32_t4 m_position;
};
struct output_fragment_st {
  float32_t4 m_color;
};

struct VOut
{
  float4 position : SV_Position;
  float3 normal;
  float3x3 tbn;
  float2 uv;
}

[shader("vertex")]
VOut vertex_shader(uint32_t a_vertex_id : SV_VertexID) {
  //static_assert((sizeof (input_vertex_st) == 48), "wtf");
  input_vertex_st vertex = ((input_vertex_st*)(push_constants.m_registers.m_registers[0]))[a_vertex_id];
  //var xxx = red();
  //let output = output_vertex_st ((vertex.m_position, 1.0));
  /*
  if (a_vertex_id < 3) {
    let offset = a_vertex_id * 0.1f;
    return {{0.0f + offset, 0.0f + offset, 0.0f + offset, 1.0f}, a_input_vertex.m_normal, a_input_vertex.m_uv};
  }
  */
  // model_view = mul(float4x4(1.0f), push_constants.m_model_transformation);
  //let mvp = mul(model_view, proj);
  let view = push_constants.m_model_transformation;
  //let model = mul(float4 (a_input_vertex.m_position.xyz, 1.0f), );
  //let model_in_view = mul((), model);
  //var model_in_ndc = mul(float4 (a_input_vertex.m_position.xyz, 1.0f), mvp);
  //float4 x =  mul(proj, mul(, ));
  //x = clamp(x, 0.0f, 1.0f);
  //let x = mul(proj, push_constants.m_model_transformation, float4 (a_input_vertex.m_position, 1.0f));
  //let id = float4x4 ();
  //let test3 = mul(view, view);
  let test = mul(view, float4 (vertex.m_position.xyz, 1.0f));
  float3 t = normalize(mul(identity, vertex.m_tangent).xyz);
  float3 n = normalize(mul(identity, float4 (vertex.m_normal, 0.0)).xyz);
  let b = cross(n,t);

  //let test2 = mul(proj, test);
  //model_in_ndc.w = clamp(model_in_ndc.w, 0.0f, 1.0f);
  return  {test, vertex.m_normal, float3x3(t,b,n),vertex.m_uv};
}

float32_t3 computeDiffuseColor(const float32_t4 baseColor, float metallic) {
    return baseColor.rgb * (1.0 - metallic);
}

[shader("fragment")]
float4 fragment_shader(VOut a_vertex, float4 pos : SV_Position) : SV_Target {
  let light_pos = float32_t3 (1.0, 1.0, 2.0);
  let light_nor = normalize(-light_pos);

  let shading_view = normalize(a_vertex.position - push_constants.m_view_data.m_view_position).xyz;
  var shading_normal = combined_image_samplers[push_constants.m_material_data.m_normal_map].Sample(a_vertex.uv).xyz;
  shading_normal = shading_normal * 2.0 - 1.0;
  shading_normal = normalize(mul(a_vertex.tbn, shading_normal));
  let NoV = dot(shading_view, shading_normal);

  let empty_metallic_roughness = combined_image_samplers[push_constants.m_material_data.m_metallic_roughness_map].Sample(a_vertex.uv);
  let metallic = empty_metallic_roughness.b;
  let base_color = computeDiffuseColor(combined_image_samplers[push_constants.m_material_data.m_base_color_map].Sample(a_vertex.uv), metallic);
  let roughness = empty_metallic_roughness.g;

  let h = normalize(shading_view + light_nor);
  let NoL = saturate(dot(shading_normal, light_nor));
  float NoH = saturate(dot(shading_normal, h));
  float LoH = saturate(dot(light_nor, h));
  //printf("im printing bro: %f", LoH);
  float F0 = 0.04;
  PixelParams pp = {base_color, roughness, roughness, F0, roughness};
  Light light = {float32_t4(1.0f, 1.0f, 1.0f, 1.0f), light_nor, 1.0, light_pos, NoL, light_nor};
  float32_t3 Fr = specularLobe(pp, light, h, NoV, NoL, NoH, LoH);
  float32_t3 Fd = diffuseLobe(pp, NoV, NoL, LoH);

  float32_t3 color = Fd + Fr;
  //return output_fragment_st ({1.0, 0.0, 0.0, 1.0});
  //return combined_image_samplers[push_constants.m_material_data.m_base_color_map].Sample(a_vertex.uv/* + sin((float32_t)push_constants.m_current_time * 0.25)*/);
  //return {a_vertex.tbn[0], 1.0};
  return {(color * light.colorIntensity.rgb) * (light.colorIntensity.w * light.attenuation * NoL) /* * light.colorIntensity.rgb*/, 1.0};
  //return {mul(-push_constants.m_model_transformation, float4(pos.xyz, 1.0f)).xyz, 1.0};
}