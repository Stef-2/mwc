module;

export module mwc_project_name_string;

import mwc_definition;

export namespace mwc {
  constexpr auto project_name_string() {
    return string_view_t {"mwc"};
  }
}