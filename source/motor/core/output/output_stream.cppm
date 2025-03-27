module;

export module mtr_output_stream;

import mtr_definition;

import std;

export namespace mtr
{
  namespace output
  {
    consteval auto output_stream() -> ostream_t& { return std::cout; }

    consteval auto error_stream() -> ostream_t& { return std::cerr; }

    consteval auto log_stream() -> ostream_t& { return std::clog; }
  }
}