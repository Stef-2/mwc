#include "mwc/core/diagnostic/log/subsystem.hpp"
#include "mwc/mwc.hpp"

import mwc_subsystem;

import vkfw;

import std;

int main() {
  mwc::initialize_subsystems();
  mwc::mwc_ct mwc {};
  while (true)
    ;
  mwc::finalize_subsystems();
  return 0;
}
