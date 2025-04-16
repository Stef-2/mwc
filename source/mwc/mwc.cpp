#include <csignal>
#include <stdio.h>

import mwc_definition;
import mwc_log;
import mwc_event_severity;
import mwc_static_map;

import std;

/*template <typename... tp_args>
  requires std::invocable<std::print(), tp_args...>
{
  std::print(a_args);
}*/

template <typename... tp_args>
/*requires std::
    invocable<std::print(), std::format_string<tp_args...>, tp_args...>*/
auto printt(const std::format_string<tp_args...>& a_format,
            tp_args&&... a_args) {
  std::print(std::forward<decltype(a_format)>(a_format),
             std::forward<tp_args>(a_args)...);
}

int main() {
  /*constexpr auto asd =
    mwc::diagnostic::log::static_configuration_st<mwc::ostream_t*> {1};
  mwc::diagnostic::log::
    log_ct<mwc::diagnostic::event_severity_et::e_information, asd>
      log;

    log.add_sink(&std::cout);
  log.write_to_sinks("Hello World!");*/

  //if (std::rand())
  //printt("fuck !!!");
  std::vector<int> a {1, 2, 3};
  //a[3] = 4;
  constexpr auto map2 =
    mwc::static_bimap_st {{mwc::pair_t {1, 2.33f}, {3, 4.76f}}};

  constexpr auto map3 = mwc::static_multimap_st<int, 3, float, 5> {
    {mwc::pair_t {1, 2.44f}, {3, 4.1f}, {1, 1.23f}, {4, 5.1f}, {3, 12.7f}}};
  printt("{0} {1}", map2[2.33f], map2[4.76f]);
  //auto um = std::unordered_map<mwc::diagnostic::event_severity_et, void*> {};

  //log.error("fuck!");
}