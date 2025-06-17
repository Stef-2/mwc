module;

export module mwc_subsystem;

import mwc_definition;
import mwc_observer_ptr;
import mwc_logging_subsystem_switch;

import std;

export namespace mwc {
  struct subsystem_st {
    subsystem_st(const initializer_list_t<obs_ptr_t<subsystem_st>> a_dependencies, const string_view_t a_name);
    subsystem_st(const subsystem_st&) = delete("construct only type");
    subsystem_st& operator=(const subsystem_st&) = delete("construct only type");
    subsystem_st(subsystem_st&&) noexcept = delete("construct only type");
    subsystem_st& operator=(subsystem_st&&) noexcept = delete("construct only type");
    virtual ~subsystem_st();

    virtual auto initialize() -> void = 0;
    virtual auto finalize() -> void = 0;

    static inline auto subsystem_registry = vector_t<obs_ptr_t<subsystem_st>> {};

    vector_t<obs_ptr_t<subsystem_st>> m_dependencies;
    string_view_t m_name;
    bool m_initialized;
  };

  auto initialize_subsystems() -> void;
  auto finalize_subsystems() -> void;
}