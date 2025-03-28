module;

export module mtr_sink;

import mtr_definition;
import mtr_extent;
import mtr_bit;
import mtr_enum_bitwise_operators;
import mtr_observer_ptr;
import mtr_output_stream;
import mtr_type_map;

import std;

export namespace mtr
{
  namespace log
  {
    enum class sink_et : uint8_t
    {
      e_console,
      e_string,
      e_file
    };

    template <sink_et t = sink_et::e_console,
              size_t stream_count = std::dynamic_extent>
    class sink_ct
    {
      public:
      /*struct configuration_st
      {
        sink_et m_type;
        extent_t<observer_ptr_t<ostream_t>, stream_count> m_streams;
      };

      static constexpr auto g_default_cfg =
        configuration_st {.m_type = sink_et::e_console,
                          .m_streams = extent_t<observer_ptr_t<ostream_t>, 1> {
                          &output::log_stream()}};

      constexpr sink_ct(const configuration_st& a_cfg = g_default_cfg)
      : m_configuration(a_cfg)
      {}

      private:
      configuration_st m_configuration;**/
    };
  }
}

namespace mtr
{
  namespace log
  {
    using console_sink_type_map_t = type_map_st<sink_et, ostream_t>;

    template <size_t stream_count>
    struct console_sink_st
    {
      extent_t<observer_ptr_t<ostream_t>, stream_count> m_streams;
    };

    template <size_t string_count>
    struct string_sink_st

    {
      extent_t<observer_ptr_t<string_t>, string_count> m_streams;
    };

    template <size_t file_count>
    struct file_sink_st
    {
      extent_t<observer_ptr_t<file_t>, file_count> m_streams;
    };
  }
}