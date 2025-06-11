//#include "mwc/core/container/data.hpp"
#include "mwc/core/diagnostic/log/default_log.hpp"
#include "mwc/core/diagnostic/log/log.hpp"
//#include <print>
//#include <source_location>

//#include <iostream>

import mwc_chrono;
import mwc_metaprogramming_utility;
import mwc_default_logging;

import std;
using namespace mwc;

int main() {
  //print("{}\n", 333);

  mwc::diagnostic::log::log_ct log {};
  log.insert_sink({static_cast<std::FILE*>(nullptr)});
  auto file = std::fopen((filepath_t {diagnostic::default_logging_directory().data()} /= "log.txt").c_str(), "w");
  std::string sss;

  using enum diagnostic::log::configuration_st::bit_flags_et;
  log.insert_sink(
    {&sss, diagnostic::event_severity_et::e_warning,
     diagnostic::log::configuration_st {
     .m_bit_mask = std::to_underlying(e_print_thread_id bitor e_print_severity_level bitor e_cover_higher_severities)}});
  log.insert_sink({file});
  for (int i = 0; i < 10; ++i) {
    //const auto fmt = std::format("{0}", std::rand());
    //log.information(std::format("rand: {0}\n", std::rand()));
  }
  error("wtf bro im writing to the CRITICAL log file bro SOMETHING REALLY BAD MUST BE HAPPENING AAAA SAVE ME NIGGERMAN");
  mwc::diagnostic::log::finalize_default_log();
  //std::cout << std::stacktrace::current();
  //mwc::data_st<int> data {&std::cout, 1};
  //std::print("{0}", mwc::chrono::now<double>());
  //std::print("{0}", mwc::chrono::now<std::size_t>());
  auto sw = mwc::chrono::stopwatch_ct();
  sw.start();
  //const auto fmt_st = sw.stop<std::chrono::duration<std::uint64_t, std::nano>>();
  std::print("time: {}", mwc::chrono::elapsed_time());
  std::print("\n{}\n", sizeof(mwc::chrono::time_point_t));
  //print_indexed<int, float>(1, 2.3f);
  //print1(std::forward_as_tuple("\n{}\n", 46));
  //print5(fmt_st {"bro its actually working: {}\n", std::rand()});
  //print(std::format("{}", 12345));
  //print1("asd {}", 2);
  //test("{}", 33);
  auto now = mwc::chrono::current_time();
  //const auto date = std::chrono::year_month_day {now};
  //std::print("{:%d.%m.%Y}", now);
  std::cout << std::format("[{:%D - %T}]", std::chrono::floor<std::chrono::milliseconds>(std::chrono::system_clock::now()))
            << "\n";
  //std::cout << date;
}
