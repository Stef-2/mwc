module;

export module mwc_log;

import mwc_definition;
import mwc_event_severity;
import mwc_concept;
import mwc_sink;

import std;

export namespace mwc
{
  namespace diagnostic
  {
    /*template <template <drain_c, size_t> typename... sinks, drain_c drain,
              size_t drain_count>*/
    template <typename... sinks>
    class log_ct
    {
      public:
      //log_ct(const auto& a_sinks) : m_sinks {a_sinks} {}

      tuple_t<sinks...> m_sinks;

      private:
    };

    void test()
    {
      // fuck off
      //sink_ct<ostream_t*, 1> sink {&std::cout};
      //log_ct<decltype(sink)> /*<ostream_t*, 1>*/ log {sink};
      log_ct<sink_ct<ostream_t*, 1>> log;
    }
  }
}