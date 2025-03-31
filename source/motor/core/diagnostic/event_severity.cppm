module;

export module mwc_event_severity;

import mwc_definition;

export namespace mwc
{
  namespace diagnostic
  {
    enum class event_severity_et : uint8_t
    {
      e_information,
      e_warning,
      e_error,
      e_critical
    };
  }
}