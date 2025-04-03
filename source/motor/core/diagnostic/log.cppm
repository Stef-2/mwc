
module;

export module mwc_log;

import mwc_definition;
import mwc_extent;
import mwc_event_severity;
import mwc_metaprogramming_utility;
import mwc_concept;
import mwc_sink;

import std;

export namespace mwc
{
  namespace diagnostic
  {

    //constexpr auto wtfff = instance_of_v<array_t<int, 1>, array_t>;

    template <typename... sinks>
      requires requires {
                 int i;
                 auto lambda = []<size_t i>()
        { /*using check_t = */sinks...[i] ::write_to_drains; };
                 static_for_loop<0, sizeof...(sinks)>(lambda);
               }

    class log_ct
    {
      public:
      //template <sinks...>
      //using drain_t = D;

      /*log_ct(
        const auto&... a_sinks)
      {}*/
      log_ct(const auto&... a_sinks) : m_sinks {a_sinks...} {}

      //log_ct() {}

      private:
      std::tuple<sinks...> m_sinks;
    };

    void test()
    {
      string_t s;
      file_t x, y, z;
      array_t a {&x, &y, &z};
      //sink_ct<ostream_t*, s_dynamic_extent> sink1 {&std::cout};
      //sink_ct<string_t*, s_dynamic_extent> sink2 {&s};
      //log_ct<decltype(sink)> /*<ostream_t*, 1>*/ log {sink};
      /*log_ct<sink_ct<ostream_t*, std::dynamic_extent>> log {
        sink_ct<ostream_t*, std::dynamic_extent> {&std::cout}};*/
      log_ct<sink_ct<ostream_t*, 1>/*, sink_ct<string_t*, 1>,
             sink_ct<file_t*, 3>*/>
        log {&std::cout/*, &s, a*/};
      //log.m_sinks.write_to_drains(" wwworking ?\n");
      //std::get<0>(log).write_to_drains(" wwworking ?\n");
    }
  }
}