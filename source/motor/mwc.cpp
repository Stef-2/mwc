#include <csignal>
#include <stdio.h>

import mwc_definition;
import mwc_log;
import mwc_event_severity;
//import mwc_sink;

import std;

/*template <typename... tp_args>
  requires std::invocable<std::print(), tp_args...>
{
  std::print(a_args);
}*/

template <typename... tp_args>
/*requires std::
    invocable<std::print(), std::format_string<tp_args...>, tp_args...>*/
auto printt(const std::format_string<tp_args...>& a_format, tp_args&&... a_args)
{
  std::print(std::forward<decltype(a_format)>(a_format),
             std::forward<tp_args>(a_args)...);
}

int main()
{
  constexpr auto asd =
    mwc::diagnostic::log::static_configuration_st<mwc::ostream_t*> {1};
  mwc::diagnostic::log::log_ct<asd> log;

  log.add_sink(&std::cout);
  log.write_to_sinks("Hello World!");

  //if (std::rand())
  //printt("fuck !!!");
  printt("{0} {1}", "fuck !!!", 123);
  auto um = std::unordered_map<mwc::diagnostic::event_severity_et, void*> {};
  std::system("cls||clear");

  //log.error("fuck!");
}