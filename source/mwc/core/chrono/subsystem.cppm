module;

export module mwc_chrono_subsystem;

import mwc_definition;
import mwc_subsystem;

import std;

export namespace mwc {
  namespace chrono {
    struct chrono_subsystem_st : public subsystem_st {
      using subsystem_st::subsystem_st;

      auto initialize() -> void override final;
      auto finalize() -> void override final;

      static inline time_point_t initialization_time;
    };

    namespace global {
      inline auto chrono_subsystem = chrono_subsystem_st {{}, "chrono subsystem"};
    }
  }
}