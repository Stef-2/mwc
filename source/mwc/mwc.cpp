//#include "mwc/core/container/data.hpp"
#include "mwc/core/diagnostic/log/log.hpp"

//#include <../../../include/llvm/Support/Compiler.h>
//#include <../../../include/llvm/Support/Signals.h>
//#include <format>
//#include <iostream>

import mwc_chrono;

import std;

template <typename... tp_args>
auto print(const std::format_string<tp_args...>& a_format, tp_args&&... a_args) {
  std::print(std::forward<decltype(a_format)>(a_format), std::forward<tp_args>(a_args)...);
}

int main() {
  //print("{}\n", 333);

  mwc::diagnostic::log::log_ct log {};
  log.insert_sink({&std::cout});
  for (int i = 0; i < 10; ++i)
    log.print("im looging bro im looing aahhhh\n", mwc::diagnostic::event_severity_et::e_information);

  //std::cout << std::stacktrace::current();
  //mwc::data_st<int> data {&std::cout, 1};
  //std::print("{0}", mwc::chrono::now<double>());
  //std::print("{0}", mwc::chrono::now<std::size_t>());
  auto sw = mwc::chrono::stopwatch_ct();
  sw.start();
  const auto t = sw.stop<std::chrono::duration<std::uint64_t, std::nano>>();
  std::print("time: {}", mwc::chrono::elapsed_time());
  std::print("\n{}\n", sizeof(mwc::chrono::time_point_t));
  print("\n{}\n", sizeof(mwc::chrono::time_point_t));
  auto now = mwc::chrono::current_time();
  //const auto date = std::chrono::year_month_day {now};
  //std::print("{:%d.%m.%Y}", now);
  std::cout << std::format("[{:%D - %T}]", std::chrono::floor<std::chrono::milliseconds>(std::chrono::system_clock::now()))
            << "\n";
  //std::cout << date;
}
