module;

export module mwc_window_subsystem;

import mwc_definition;
import mwc_subsystem;
import mwc_log_subsystem;

import vkfw;

export namespace mwc {
  struct window_subsystem_st : public subsystem_st {
    using subsystem_st::subsystem_st;

    auto initialize() -> void override final;
    auto finalize() -> void override final;
  };

  namespace global {
    inline auto window_subsystem
      = window_subsystem_st {{&diagnostic::log::global::logging_subsystem}, string_view_t {"window subsystem"}};
  }
}