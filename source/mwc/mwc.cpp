//#include "mwc/core/contract/natural_syntax.hpp"
//#include <format>
#include <mwc/core/container/static_bi_multimap.hpp>
#include <print>
#include <iostream>
#include <contracts>
#include <array>
//#include <print>
//#include <experimental/contract>
import mwc_optional;
//import mwc_definition;
//import mwc_log;
//import mwc_event_severity;
//import mwc_static_bi_map;
//import mwc_concept;
//import mwc_output_stream;
//import mwc_static_bi_multimap;
//import mwc_contract_violation;
//import mwc_data;
//#include <iostream>
//import mwc_test;
//import std;

//import mwc_test;
//template <typename... tp_args>
/*requires std::
    invocable<std::print(), std::format_string<tp_args...>, tp_args...>*/
/*auto printt(const std::format_string<tp_args...>& a_format, tp_args&&... a_args) {
  std::print(std::forward<decltype(a_format)>(a_format),
             std::forward<tp_args>(a_args)...);
}*/

// done testing
void handle_contract_violation(const std::contracts::contract_violation& a_violation) {std::cerr << "contract got violated again:\n" << a_violation.comment();}
int test(int i) pre(i > 0) {return 1 / i;}
int main()  {
  std::cout << "hello world\n" << test(2);
  mwc::optional_st<int> opt {1};
  std::cout << opt.m_data << '\n';
  //test2(2);
  //wtf w {3.0f};
  //w.print();
  std::print("wtf");
  //test2();
  //std::set_terminate([]() { std::abort(); });
  //std::print("my god: \n");
  //fn(0);
  /*constexpr auto asd =
    mwc::diagnostic::log::static_configuration_st<mwc::ostream_t*> {1};
  mwc::diagnostic::log::
    log_ct<mwc::diagnostic::event_severity_et::e_information, asd>
      log;

    log.add_sink(&std::cout);
  log.write_to_sinks("Hello World!");*/

  //if (std::rand())
  //printt("fuck !!!");
  //std::vector<int> a {1, 2, 3};
  //a[3] = 4;
  //auto map2 = static_unordered_bi_map_st {{mwc::pair_t {1, 2.33f}, {3, 4.76f}}};
  //auto wtf = map2[1];
  mwc::array_t<mwc::pair_t<int, float>, 2> asad {mwc::pair_t{1, 2.33f}, {3, 4.76f}};
  const mwc::span_t span {asad};
  mwc::static_bi_multimap_st<int, 2, float, 2> map {span};
  const auto wtf = map.equal_range(1);
  std::cout << *(wtf.first);
  /*static constexpr auto map3 = mwc::static_bi_multimap_st<int, 3, float, 5> {
    {mwc::pair_t {1, 2.44f}, {3, 4.1f}, {1, 1.23f}, {4, 5.1f}, {3, 12.7f}}};
  constexpr int i = map3[2.44f];
  //std::span<const float> f = map3[4];
  static constexpr std::array<const int, 3> arr = {1, 2, 3};
  constexpr auto w = std::span {arr};
  static constexpr auto f = map3[3];
  fn(-1);*/
  //std::print("{0} {1}", i, w);
  //auto um = std::unordered_map<mwc::diagnostic::event_severity_et, void*> {};

  //log.error("fuck!");

  //mwc::diagnostic::log::testtt();
}
