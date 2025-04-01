
module;

export module mwc_log;

import mwc_definition;
import mwc_extent;
import mwc_event_severity;
import mwc_concept;
import mwc_sink;

import std;

export namespace mwc
{
  namespace diagnostic
  {

    //template <template <drain_c c, size_t n> typename... sinks>
    //using wtf = sinks;

    /*template <template <drain_c, size_t> typename... sinks, drain_c drain,
              size_t drain_count>*/
    //template <template <drain_c, size_t> typename... sinks>
    template <sink_ct<drain_c, size_t>... sinks>
    class log_ct
    {
      public:
      //template <sinks...>
      //using drain_t = D;

      /*log_ct(
        const auto&... a_sinks)
      {}*/

      std::tuple<decltype(sinks)...> m_sinks;

      private:
    };

    void test()
    {

      //sink_ct<ostream_t*, 1> sink {&std::cout};
      //log_ct<decltype(sink)> /*<ostream_t*, 1>*/ log {sink};
      /*log_ct<sink_ct<ostream_t*, std::dynamic_extent>> log {
        sink_ct<ostream_t*, std::dynamic_extent> {&std::cout}};*/
      log_ct<sink_ct<ostream_t*, 1>> log {
        /*sink_ct<ostream_t*, 1> {&std::cout}*/};
      //log.m_sinks.write_to_drains(" wwworking ?\n");
      //std::get<0>(log).write_to_drains(" wwworking ?\n");
    }
  }
}