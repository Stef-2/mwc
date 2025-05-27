#include "mwc/core/contract/natural_syntax.hpp"
//#include <experimental/contract>

import mwc_definition;
import mwc_log;
import mwc_event_severity;
import mwc_static_bi_map;
import mwc_concept;
//import mwc_static_bi_multimap;
//import mwc_contract_violation;
import mwc_data;

import std;
using namespace mwc;
using namespace mwc::diagnostic;
using namespace mwc::diagnostic::log;
void testtt() {
  static string_t s;
  //file_t x, y, z;
  //array_t a {&x, &y, &z};

  static constexpr auto sinks =
    array_t<pair_t<event_severity_et, sink_st>, 1> {pair_t {
    event_severity_et::e_error, sink_st {&s, sink_st::sink_et::e_string}}};
  std::cout << "address of std::cout: " << &std::cout << '\n';
  static constexpr auto scfg = configuration_st<static_configuration_st {1, 1}> {
    .m_sinks = data_st<pair_t<event_severity_et, sink_st>> {std::span(sinks)}};

  static const log_ct<scfg> slog;
  std::cout << "address of the first sink: " << slog.m_storage.m_values[0].m_ptr
            << '\n';
  slog.print("im so static bro im an error brooo ahhh",
             event_severity_et::e_error);
  auto dcfg = configuration_st<dynamic_configuration_st {}> {};
  static log_ct dlog {dcfg};
  dlog.insert_sink(sinks[0].second, event_severity_et::e_error);
  //dlog.remove_sink(sinks[0].second);
  //dlog.remove_severity_level(event_severity_et::e_error);
  dlog.print("hello my ERRONEOUS niggas", event_severity_et::e_error);
}

/*void handle_contract_violation(
  const std::experimental::contract_violation& a_violation) {
  std::print("contract violation: {0}\n", a_violation.comment());
}*/

/*template <typename... tp_args>
  requires std::invocable<std::print(), tp_args...>
{
  std::print(a_args);
}*/

//void test_contracts(int wtf)(wtf > 0);

/*int fn(int x) /*pre(x > 0) post(r : r !=
                                    0)*/
/*[[pre:x > 0]][[post r:r != 0]]*/ /*{
  contract_assert(x != 0);
  return 1;
}*/

//template <typename... tp_args>
/*requires std::
    invocable<std::print(), std::format_string<tp_args...>, tp_args...>*/
/*auto printt(const std::format_string<tp_args...>& a_format, tp_args&&... a_args) {
  std::print(std::forward<decltype(a_format)>(a_format),
             std::forward<tp_args>(a_args)...);
}*/

// done testing

int main() {
  std::print("my god: \n");
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
  std::vector<int> a {1, 2, 3};
  //a[3] = 4;
  //auto map2 = static_unordered_bi_map_st {{mwc::pair_t {1, 2.33f}, {3, 4.76f}}};
  //auto wtf = map2[1];

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
  testtt();
}
