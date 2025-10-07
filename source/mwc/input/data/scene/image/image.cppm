module;

export module mwc_input_image;

import mwc_definition;
import mwc_input_resource;

export namespace mwc {
  namespace input {
    template <size_t tp_width, size_t tp_height, size_t tp_channel_count>
    struct static_image_st : public resource_st {
      using storage_t = vector_t<byte_t>;

      static constexpr auto width = tp_width;
      static constexpr auto height = tp_height;
      static constexpr auto channel_count = tp_channel_count;

      storage_t m_data;
    };

    struct dynamic_image_st : public resource_st {
      using storage_t = vector_t<byte_t>;
      using dimension_t = uint32_t;
      using channel_count_t = uint32_t;

      storage_t m_data;
      dimension_t m_width;
      dimension_t m_height;
      channel_count_t m_channel_count;
    };
  }
}