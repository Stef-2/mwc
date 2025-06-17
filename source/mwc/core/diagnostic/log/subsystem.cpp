#include "mwc/core/diagnostic/log/subsystem.hpp"

namespace mwc {
  namespace diagnostic {
    namespace log {
      auto log_subsystem_st::initialize() -> void {
        std::println("initializing {0}", m_name);
        std::println("redirecting diagnostics to the standard output until the {0} is initialized", m_name);
        using enum configuration_st::bit_flags_et;
        constexpr auto sink_count = 5;
        // initialize log files
        // one for each event severity level
        auto log_files = array_t<file_ptr_t, std::to_underlying(event_severity_et::end)> {};
        auto sinks = array_t<sink_st, sink_count> {file_ptr_t {nullptr}, file_ptr_t {nullptr}, file_ptr_t {nullptr},
                                                   file_ptr_t {nullptr}, ostream_ptr_t {nullptr}};

        std::println("generating {0} sinks for the {1}:", sink_count, m_name);
        for (auto i = std::underlying_type_t<event_severity_et> {0}; i < std::to_underlying(event_severity_et::end); ++i) {
          const auto sink_path =
            (filepath_t {logging_subsystem_directory()} /= event_severity_level_string(event_severity_et {i})) +=
            file_type_extension<file_type_et::e_text>();
          log_files[i] = std::fopen(sink_path.c_str(), "w");
          contract_assert(log_files[i]);
          // note: change this to proper std::filesystem::path formatting once clang implements it
          std::println("[{0}] {1}", i, sink_path.string());

          constexpr auto log_file_configuration =
            configuration_st {std::to_underlying(e_print_timestamp bitor e_print_severity_level bitor e_print_thread_id bitor
                                                 e_print_source_location bitor e_print_stacktrace)};
          sinks[i] = {log_files[i], event_severity_et {i}, log_file_configuration};
        }
        constexpr auto standard_output_stream_configuration =
          configuration_st {std::to_underlying(e_print_timestamp bitor e_print_severity_level bitor e_print_thread_id bitor
                                               e_print_source_location bitor e_print_stacktrace bitor e_cover_higher_severities)};
        sinks.back() = {&output::log_stream(), event_severity_et::e_information, standard_output_stream_configuration};
        std::println("[{0}] {1} at address {2}", sink_count - 1, "standard log output", static_cast<void*>(&output::log_stream()));

        m_log = log_ct {sinks};
        std::println("{0} initialized", m_name);
        m_initialized = true;
      }
    }
  }
  template <>
  auto information<true>(const string_view_t a_message, const std::source_location& a_source_location)
    -> void pre(contract::validate_storage(a_message)) {
    diagnostic::log::global::logging_subsystem.m_log.information(a_message, a_source_location);
  }
  template <>
  auto warning<true>(const string_view_t a_message, const std::source_location& a_source_location)
    -> void pre(contract::validate_storage(a_message)) {
    diagnostic::log::global::logging_subsystem.m_log.warning(a_message, a_source_location);
  }
  template <>
  auto error<true>(const string_view_t a_message, const std::source_location& a_source_location)
    -> void pre(contract::validate_storage(a_message)) {
    diagnostic::log::global::logging_subsystem.m_log.error(a_message, a_source_location);
  }
  template <>
  auto critical<true>(const string_view_t a_message, const std::source_location& a_source_location)
    -> void pre(contract::validate_storage(a_message)) {
    diagnostic::log::global::logging_subsystem.m_log.critical(a_message, a_source_location);
  }
}

namespace mwc {
  namespace diagnostic {
    namespace log {
      auto log_subsystem_st::finalize() -> void {
        information(std::format("finalizing {0}", m_name));
        for (const auto& sink : m_log.storage())
          if (sink.m_type == sink_st::sink_et::e_file)
            std::fclose(static_cast<file_ptr_t>(sink.m_sink_ptr));
        information(std::format("{0} finalized", m_name));
        m_initialized = false;
      }
    }
  }
}