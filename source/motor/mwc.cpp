#include <csignal>
#include <stdio.h>

import mwc_definition;
import mwc_log;
import mwc_sink;

import std;

int main()
{
  constexpr auto asd =
    mwc::diagnostic::log::static_configuration_st<mwc::ostream_t*> {1};
  mwc::diagnostic::log::log_ct<asd> log;

  log.add_sink(&std::cout);
  log.write_to_sinks("Hello World!");
}