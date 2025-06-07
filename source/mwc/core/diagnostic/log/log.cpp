#include "mwc/core/diagnostic/log/log.hpp"

import mwc_chrono;

namespace {
  constexpr auto event_severity_level_string(const mwc::diagnostic::event_severity_et a_event_severity)
    pre(a_event_severity != mwc::diagnostic::event_severity_et::end) {
    using enum mwc::diagnostic::event_severity_et;

    switch (a_event_severity) {
      case e_information : return "information";
      case e_warning : return "warning";
      case e_error : return "error";
      case e_critical : return "critical";
      default : contract_assert(false); std::unreachable();
    }
  }
}

namespace mwc::diagnostic::log {
  constexpr auto sink_st::operator==(const sink_st& a_other) const -> bool {
    return m_sink_ptr == a_other.m_sink_ptr;
  }
  constexpr auto sink_st::operator==(const sink_c auto a_sink) const -> bool {
    return m_sink_ptr == a_sink;
  }
  auto sink_st::print(const string_view_t a_string) const -> void {
    switch (m_type) {
      case sink_et::e_ostream : std::print(*static_cast<ostream_ptr_t>(m_sink_ptr), "{0}", a_string); break;
      case sink_et::e_string : static_cast<string_ptr_t>(m_sink_ptr)->append(a_string); break;
      case sink_et::e_file : std::print(static_cast<file_ptr_t>(m_sink_ptr), "{0}", a_string); break;
    }
  }
  log_ct::log_ct(const span_t<sink_st> a_sinks, const event_severity_et a_default_severity_level, const configuration_st& a_cfg)
  : m_storage {a_sinks.begin(), a_sinks.end()},
    m_default_severity_level {a_default_severity_level},
    m_configuration {a_cfg} {}
  auto log_ct::configuration(this auto&& a_this) -> configuration_st& {
    return a_this.m_configuration;
  }
  auto log_ct::insert_sink(const sink_st& a_sink) -> void {
    m_storage.emplace_back(a_sink);
  }
  auto log_ct::remove_sink(const sink_st& a_sink) -> void {
    std::erase(m_storage, a_sink);
  }
  auto log_ct::print(const string_view_t a_string, const optional_t<event_severity_et> a_event_severity,
                     const std::source_location& a_source_location) const -> void {
    using enum configuration_st::bit_flags_et;

    // use the passed event severity level if provided
    // otherwise, use the default one
    const auto event_severity = a_event_severity.has_value() ? a_event_severity.value() : m_default_severity_level;

    // form an estimate on how large prefixed data is in terms of character length
    // this can be used later when reserving storage for the fully formed string
    // thus avoiding unnecessary heap allocations and reallocations
    constexpr auto prefix_length =
      string_view_t {"[1993-01-22 - 03:40:00] thread 00000000 some_file_name.cpp average_function_name 96:81"}.length();

    using timestamp_precision_t = std::chrono::seconds;

    // data
    const auto time = std::chrono::floor<timestamp_precision_t>(std::chrono::system_clock::now());
    const auto thread_id = std::this_thread::get_id();

    for (const auto& sink : m_storage) {
      // if the sink has its own configuration, use it, otherwise, use the logger's one
      const auto& configuration = sink.m_configuration.has_value() ? sink.m_configuration.value() : m_configuration;

      // event severity level matches exactly
      const auto matching_severity_level = sink.m_event_severity == event_severity;
      // the sink is also subscribed to higher event severity levels
      const auto covering_higher_severity_level = sink.m_event_severity > event_severity and
                                                  configuration.m_bit_mask bitand std::to_underlying(e_cover_higher_severities);

      if (matching_severity_level or covering_higher_severity_level) {
        // generate and preallocate the buffer storage
        auto buffer = string_t {};
        buffer.reserve(prefix_length + a_string.length());

        // begin assembly
        constexpr auto prefix_delimiter = ' ';
        constexpr auto prefix_punctuator = ':';

        // timestamp
        if (configuration.m_bit_mask bitand std::to_underlying(e_print_timestamp))
          std::format_to(std::back_inserter(buffer), "[{0:%D - %T}]{1}", time, prefix_delimiter);
        // event severity level
        if (configuration.m_bit_mask bitand std::to_underlying(e_print_severity_level))
          std::format_to(std::back_inserter(buffer), "[{0}]{1}", event_severity_level_string(event_severity), prefix_delimiter);
        // thread id
        if (configuration.m_bit_mask bitand std::to_underlying(e_print_thread_id))
          std::format_to(std::back_inserter(buffer), "[thread{2}{0}]{1}", thread_id, prefix_delimiter, prefix_punctuator);
        // source location
        if (configuration.m_bit_mask bitand std::to_underlying(e_print_source_location))
          std::format_to(std::back_inserter(buffer), "[source{5}{0} at: {1}{4}{2}{5}{3}]", a_source_location.file_name(),
                         a_source_location.function_name(), a_source_location.line(), a_source_location.column(),
                         prefix_delimiter, prefix_punctuator);
        // stacktrace
        // note: add support for stacktrace printing once clang implements <stacktrace>

        // message
        buffer.append("\n").append(a_string);
        sink.print(buffer);
      }
    }
  }
}