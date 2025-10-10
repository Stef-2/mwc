module;

export module mwc_host_image;

import mwc_definition;

export namespace mwc {
  namespace input {
    template <size_t tp_width, size_t tp_height, size_t tp_channel_count>
    struct static_host_image_st {
      using storage_t = vector_t<byte_t>;

      static constexpr auto width = tp_width;
      static constexpr auto height = tp_height;
      static constexpr auto channel_count = tp_channel_count;

      storage_t m_data;
    };

    struct dynamic_host_image_st {
      using storage_t = vector_t<byte_t>;
      using dimension_t = uint32_t;
      using channel_count_t = uint32_t;

      storage_t m_data;
      size_t m_data_byte_count;
      dimension_t m_width;
      dimension_t m_height;
      channel_count_t m_channel_count;
    };
  }
}