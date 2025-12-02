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

  float32_t4x4 m_model_transformation;
  view_data_st m_view_data;
  projection_data_st m_projection_data;
  buffer_device_address_registers_st m_registers;
  float64_t m_current_time = {};
  float64_t m_delta_time = {};
};

[vk::push_constant]
uniform push_constants_st push_constants;

uniform Sampler2D combined_image_samplers[3];

static const float4x4 proj = {0.5627, 0, 0, 0, 0, 1, 0, 0, 0, 0, -1.0002, -1, 0, 0, -0.2000, 0};
static const float4x4 proj2 = {0.5625, 0, 0, 0, 0, 1, 0, 0, 0, 0, -1.0000, -1, 0, 0, -0.0200, 0};
static const float4x4 proj3 = {0.5625, 0, 0, 0, 0, 1, 0, 0, 0, 0, -1.0101, -1, 0, 0, -0.2010, 0};

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
  let model_view = mul(float4x4(1.0f), push_constants.m_model_transformation);
  let mvp = mul(model_view, proj);
  let view = push_constants.m_model_transformation;
  //let model = mul(float4 (a_input_vertex.m_position.xyz, 1.0f), );
  //let model_in_view = mul((), model);
  //var model_in_ndc = mul(float4 (a_input_vertex.m_position.xyz, 1.0f), mvp);
  //float4 x =  mul(proj, mul(, ));
  //x = clamp(x, 0.0f, 1.0f);
  //let x = mul(proj, push_constants.m_model_transformation, float4 (a_input_vertex.m_position, 1.0f));
  let test = mul(view, float4 (vertex.m_position.xyz, 1.0f));
  let test2 = mul(proj, test);
  //model_in_ndc.w = clamp(model_in_ndc.w, 0.0f, 1.0f);
  return  {test, vertex.m_normal, vertex.m_uv};
}

[shader("fragment")]
float4 fragment_shader(VOut a_vertex) : SV_Target {
  //return output_fragment_st ({1.0, 0.0, 0.0, 1.0});
  return combined_image_samplers[0].Sample(a_vertex.uv + sin((float32_t)push_constants.m_current_time * 0.25));
}