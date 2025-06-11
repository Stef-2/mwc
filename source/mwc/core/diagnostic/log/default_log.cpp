#include "mwc/core/diagnostic/log/default_log.hpp"

namespace mwc {
  namespace diagnostic {
    namespace log {
      auto initialize_default_log() -> default_log_t {
        using enum configuration_st::bit_flags_et;
        constexpr auto sink_count = 5;
        // initialize log files
        // one for each event severity level
        auto log_files = array_t<file_ptr_t, std::to_underlying(event_severity_et::end)> {};
        auto sinks = array_t<sink_st, sink_count> {file_ptr_t {nullptr}, file_ptr_t {nullptr}, file_ptr_t {nullptr},
                                                   file_ptr_t {nullptr}, ostream_ptr_t {nullptr}};

        for (auto i = std::underlying_type_t<event_severity_et> {0}; i < std::to_underlying(event_severity_et::end); ++i) {
          log_files[i] =
            std::fopen(((filepath_t {default_logging_directory()} /= event_severity_level_string(event_severity_et {i})) +=
                        file_type_extension<file_type_et::e_text>())
                         .c_str(),
                       "w");
          contract_assert(log_files[i]);

          constexpr auto log_file_configuration =
            configuration_st {std::to_underlying(e_print_timestamp bitor e_print_severity_level bitor e_print_thread_id bitor
                                                 e_print_source_location bitor e_print_stacktrace)};
          sinks[i] = {log_files[i], event_severity_et {i}, log_file_configuration};
        }
        constexpr auto standard_output_stream_configuration =
          configuration_st {std::to_underlying(e_print_timestamp bitor e_print_severity_level bitor e_print_thread_id bitor
                                               e_print_source_location bitor e_print_stacktrace bitor e_cover_higher_severities)};
        sinks.back() = {&output::log_stream(), event_severity_et::e_information, standard_output_stream_configuration};

        return log_ct {sinks};
      }
      auto finalize_default_log() -> void {
        for (const auto& sink : s_default_log.storage())
          if (sink.m_type == sink_st::sink_et::e_file)
            std::fclose(static_cast<file_ptr_t>(sink.m_sink_ptr));
      }
    }
  }
  auto information(const string_view_t a_message, const std::source_location& a_source_location)
    -> void pre(contract::validate_storage(a_message)) {
    diagnostic::log::s_default_log.information(a_message, a_source_location);
  }
  auto warning(const string_view_t a_message, const std::source_location& a_source_location)
    -> void pre(contract::validate_storage(a_message)) {
    diagnostic::log::s_default_log.warning(a_message, a_source_location);
  }
  auto error(const string_view_t a_message, const std::source_location& a_source_location)
    -> void pre(contract::validate_storage(a_message)) {
    diagnostic::log::s_default_log.error(a_message, a_source_location);
  }
  auto critical(const string_view_t a_message, const std::source_location& a_source_location)
    -> void pre(contract::validate_storage(a_message)) {
    diagnostic::log::s_default_log.critical(a_message, a_source_location);
  }
}