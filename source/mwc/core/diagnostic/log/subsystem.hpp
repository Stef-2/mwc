#pragma once

#include "mwc/core/contract/definition.hpp"

#include "mwc/core/chrono/subsystem.hpp"
#include "mwc/core/diagnostic/log/log.hpp"
#include "mwc/core/filesystem/file_type.hpp"

import mwc_subsystem;
import mwc_empty_type;

import std;

namespace mwc {
  namespace diagnostic {
    namespace log {
      struct log_subsystem_st : public subsystem_st {
        using log_t = std::conditional_t<logging_subsystem_switch(), log_ct, empty_st>;
        using string_sink_t = std::conditional_t<logging_subsystem_switch(), string_t, empty_st>;

        using subsystem_st::subsystem_st;

        auto initialize() -> void override final;
        auto finalize() -> void override final;

        static inline auto string_sink = string_sink_t {};
        static inline auto log = log_t {};
      };

      namespace global {
        inline auto logging_subsystem = log_subsystem_st {{&chrono::global::chrono_subsystem}, string_view_t {"log subsystem"}};
      }
    }
  }
}