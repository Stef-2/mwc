module;

export module mtr_log;

import mtr_definition;

import std;

export namespace mtr
{
  namespace log
  {
    enum class severity_et : uint8_t
    {
      e_information,
      e_warning,
      e_error,
      e_critical
    };
  }
}