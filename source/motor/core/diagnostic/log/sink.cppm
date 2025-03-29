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

namespace mtr
{
  namespace log
  {
    // sink types
    export enum class sink_et : uint8_t
    {
      e_console,
      e_string,
      e_file
    };

    template <typename T>
    concept sink_output_t = concepts::any_off<T, ostream_t, string_t, file_t>;

    // map sink types to their respective output stream types
    template <sink_et sink_type = sink_et::e_console>
    auto constexpr sink_output_map()
    {
      if constexpr (sink_type == sink_et::e_console)
        return observer_ptr_t<std::remove_reference_t<decltype(output::log_stream())>> {};
      if constexpr (sink_type == sink_et::e_string)
        return observer_ptr_t<string_t> {};
      if constexpr (sink_type == sink_et::e_file)
        return observer_ptr_t<file_t> {};
    }

    export template <sink_et sink_type = sink_et::e_console,
                     size_t stream_count = 1>
    class sink_ct
    {
      public:
      using sink_output_t = decltype(sink_output_map<sink_type>());

      constexpr sink_ct(const extent_t<sink_output_t, stream_count>& a_streams /*= {&output::log_stream()}*/)
      : m_streams {std::move(a_streams)}
      {}

      private:
      extent_t<sink_output_t, stream_count> m_streams;
    };

    auto unit_test()
    {
      string_t s;
      auto sink = sink_ct {{&s}};
    }
  }
}