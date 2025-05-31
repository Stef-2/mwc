module;

#include <mwc/core/definition/definition.hpp>

export module mwc_event_severity;

export namespace mwc {
  namespace diagnostic {

    enum class event_severity_et : uint8_t {
      e_information,
      e_warning,
      e_error,
      e_critical,
      end
    };
  }
}
