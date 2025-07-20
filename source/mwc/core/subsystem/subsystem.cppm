module;

export module mwc_subsystem;

import mwc_definition;
import mwc_observer_ptr;
import mwc_type_mobility;
import mwc_logging_subsystem_switch;

import std;

export namespace mwc {
  struct subsystem_st : public immovable_st {
    subsystem_st(const initializer_list_t<obs_ptr_t<subsystem_st>> a_dependencies, const string_view_t a_name);
    virtual ~subsystem_st();

    virtual auto initialize() -> void = 0;
    virtual auto finalize() -> void = 0;
    auto depends_on(const subsystem_st& a_subsystem) -> bool_t;

    static inline auto subsystem_registry = vector_t<obs_ptr_t<subsystem_st>> {};

    vector_t<obs_ptr_t<subsystem_st>> m_dependencies;
    string_view_t m_name;
    bool_t m_initialized;
  };

  auto initialize_subsystems() -> void;
  auto finalize_subsystems() -> void;
}