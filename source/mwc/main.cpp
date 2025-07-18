#include "mwc/mwc.hpp"
#include "mwc/core/filesystem/subsystem.hpp"
#include "mwc/input/subsystem.hpp"

import mwc_subsystem;

import std;

import vkfw;

int main() {
  mwc::initialize_subsystems();
  mwc::mwc_ct mwc {};

  while (true) {
    vkfw::pollEvents();
    mwc.m_graphics.render();
  }

  mwc::finalize_subsystems();

  return 0;
}
