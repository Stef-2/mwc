#include <experimental/contract>

import mwc_definition;
import mwc_log;
import mwc_event_severity;
import mwc_static_bi_map;
import mwc_static_bi_multimap;

import std;

/*template <typename... tp_args>
  requires std::invocable<std::print(), tp_args...>
{
  std::print(a_args);
}*/

//void test_contracts(int wtf)(wtf > 0);

int f(int x)[[expects audit:x > 0]] { return 1 / x; }

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
    mwc::static_bi_map_st {{mwc::pair_t {1, 2.33f}, {3, 4.76f}}};

  static constexpr auto map3 = mwc::static_bi_multimap_st<int, 3, float, 5> {
    {mwc::pair_t {1, 2.44f}, {3, 4.1f}, {1, 1.23f}, {4, 5.1f}, {3, 12.7f}}};
  constexpr int i = map3[2.44f];
  //std::span<const float> f = map3[4];
  static constexpr std::array<const int, 3> arr = {1, 2, 3};
  constexpr auto w = std::span {arr};
  static constexpr auto f = map3[3];
  test_contracts(666);
  //std::print("{0} {1}", i, w);
  //auto um = std::unordered_map<mwc::diagnostic::event_severity_et, void*> {};

  //log.error("fuck!");
}
