module;
//#include <iostream>
export module mwc_test;

//import mwc_output_stream;
import mwc_event_severity;
import mwc_log;
import std;
export void test() noexcept {
  using namespace mwc;
  using namespace mwc::diagnostic;
  using namespace mwc::diagnostic::log;
  //auto dcfg = configuration_st<dynamic_configuration_st {}> {};
  log_ct dlog {{}};
  dlog.insert_sink({&std::cout, sink_st::sink_et::e_ostream},
                   event_severity_et::e_error);
  //dlog.remove_sink(sinks[0].second);
  //dlog.remove_severity_level(event_severity_et::e_error);
  dlog.print("hello my ERRONEOUS niggas", event_severity_et::e_error);
}