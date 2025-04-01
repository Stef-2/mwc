#include <csignal>
#include <stdio.h>

import mwc_definition;
import mwc_log;
import mwc_sink;

import std;

int main()
{
  // this is the main entry point for the application
  mwc::string_t wtf = {"asd"};
  std::println("ait");

  std::println("{0}", MWC_LOG_DIR);
  // mwc::log::wtf();
  //mwc::log::sink_ct<mwc::log::sink_et::e_console, 1> wtf_sink;
  char* omg = nullptr;
  std::string s = "sinksss";
  std::FILE* f = std::fopen("sink.txt", "w");
  mwc::diagnostic::sink_ct sink {&s};
  sink.write_to_drains(" wwworking ?\n");

  //int i = 3 / 0;
  //std::cout << i;
  const bool aaa = MWC_DEBUG;
  std::cout << s;
}