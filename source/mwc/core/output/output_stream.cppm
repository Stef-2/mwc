module;

#include "mwc/core/definition/definition.hpp"

#include <iostream>

export module mwc_output_stream;

export namespace mwc
{
  namespace output
  {
    consteval auto output_stream() -> ostream_t& { return std::cout; }

    consteval auto error_stream() -> ostream_t& { return std::cerr; }

    consteval auto log_stream() -> ostream_t& { return std::clog; }
  }
}