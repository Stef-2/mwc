
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

    template <typename... ts>
    class log_ct
    {
      public:
      static constexpr auto ostream =
        concepts::any_of_c<ts..., sink_ct<ostream_t*, s_dynamic_extent>>;

      //template <sinks...>
      //using drain_t = D;

      /*log_ct(
        const auto&... a_sinks)
      {}*/
      //log_ct(const auto&... a_sinks) /*: m_sinks {a_sinks...} */ {}
      //log_ct() {}
      log_ct(const auto&... a_sinks) : m_sinks {std::make_tuple(a_sinks...)} {}

      private:
      std::tuple<ts...> m_sinks;
    };

    void test()
    {
      string_t s;
      sink_ct<ostream_t*, s_dynamic_extent> sink1 {&std::cout};
      sink_ct<string_t*, s_dynamic_extent> sink2 {&s};
      //log_ct<decltype(sink)> /*<ostream_t*, 1>*/ log {sink};
      /*log_ct<sink_ct<ostream_t*, std::dynamic_extent>> log {
        sink_ct<ostream_t*, std::dynamic_extent> {&std::cout}};*/
      log_ct<sink_ct<ostream_t*, s_dynamic_extent>> log {sink1};
      //log.m_sinks.write_to_drains(" wwworking ?\n");
      //std::get<0>(log).write_to_drains(" wwworking ?\n");
    }
  }
}