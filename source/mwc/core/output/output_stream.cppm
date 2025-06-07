module;

//#include "mwc/core/definition/definition.cppm"

//#include <iostream>

export module mwc_output_stream;

import mwc_definition;

import std;

export namespace mwc
{
  namespace output
  {
    consteval auto output_stream() -> ostream_t& { return std::cout; }

    consteval auto error_stream() -> ostream_t& { return std::cerr; }

    consteval auto log_stream() -> ostream_t& { return std::clog; }
  }
}