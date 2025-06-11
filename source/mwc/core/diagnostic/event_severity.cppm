module;

export module mwc_event_severity;

import mwc_definition;

import std;

export namespace mwc {
  namespace diagnostic {

    // severity of events
    // note: assumed by dependant subsystems to have strictly increasing values
    enum class event_severity_et : uint8_t { e_information, e_warning, e_error, e_critical, end };

    constexpr auto event_severity_level_string(event_severity_et a_event_severity_level) {
      using enum event_severity_et;

      contract_assert(a_event_severity_level != event_severity_et::end);

      switch (a_event_severity_level) {
        case e_information : return "information";
        case e_warning : return "warning";
        case e_error : return "error";
        case e_critical : return "critical";
        default : contract_assert(false); std::unreachable();
      }
    }
  }
}
