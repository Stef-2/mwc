module;

export module mwc_subsystem;

import mwc_definition;
import mwc_observer_ptr;
import mwc_default_logging;

import std;

export namespace mwc {
  struct subsystem_st {
    subsystem_st(const initializer_list_t<obs_ptr_t<subsystem_st>> a_dependencies, const string_view_t a_name);
    virtual ~subsystem_st() = default;

    virtual auto initialize() -> void = 0;
    virtual auto finalize() -> void = 0;

    vector_t<obs_ptr_t<subsystem_st>> m_dependencies;
    string_view_t m_name;
    bool m_initialized;
  };

  struct root_subsystem_st : subsystem_st {
    using subsystem_st::subsystem_st;

    auto initialize() -> void override final;
    auto finalize() -> void override final;

    static inline vector_t<obs_ptr_t<subsystem_st>> s_subsystem_registry {};
  };

  inline auto s_root_subsystem = root_subsystem_st {{nullptr}, string_view_t {"root subsystem"}};
}