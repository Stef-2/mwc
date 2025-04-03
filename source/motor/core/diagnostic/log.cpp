module;

module mwc_log;

namespace mwc
{
  namespace diagnostic
  {
    void test()
    {
      string_t s;
      //sink_ct<ostream_t*, s_dynamic_extent> sink1 {&std::cout};
      //sink_ct<string_t*, s_dynamic_extent> sink2 {&s};
      //log_ct<decltype(sink)> /*<ostream_t*, 1>*/ log {sink};
      /*log_ct<sink_ct<ostream_t*, std::dynamic_extent>> log {
        sink_ct<ostream_t*, std::dynamic_extent> {&std::cout}};*/
      log_ct<sink_ct<ostream_t*, s_dynamic_extent>,
             sink_ct<string_t*, s_dynamic_extent>>
        log {{&std::cout, &s}};
      //log.m_sinks.write_to_drains(" wwworking ?\n");
      //std::get<0>(log).write_to_drains(" wwworking ?\n");
    }
  }
}