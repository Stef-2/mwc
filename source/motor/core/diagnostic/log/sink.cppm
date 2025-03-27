module;

export module mtr_sink;

import mtr_definition;
import mtr_extent;
import mtr_bit;
import mtr_enum_bitwise_operators;
import mtr_observer_ptr;
import mtr_output_stream;

import std;

export namespace mtr
{
  namespace log
  {
    enum class sink_bitmask_et : uint8_t
    {
      e_console = utility::bit<1>(),
      e_string = utility::bit<2>(),
      e_file = utility::bit<3>()
    };

    template <sink_bitmask_et t = sink_bitmask_et::e_console>
    class sink_ct
    {
      public:
      struct configuration_st
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
      configuration_st m_configuration;
    };
  }
}