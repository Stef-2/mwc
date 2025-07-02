#include "mwc/core/diagnostic/log/log.hpp"

namespace mwc::diagnostic::log {
  constexpr auto sink_st::operator==(const sink_st& a_other) const -> bool_t {
    return m_sink_ptr == a_other.m_sink_ptr;
  }
  constexpr auto sink_st::operator==(const sink_c auto a_sink) const -> bool_t {
    return m_sink_ptr == a_sink;
  }
  auto sink_st::print(const string_view_t a_message) const -> void {
    switch (m_type) {
      case sink_et::e_ostream : std::println(*static_cast<ostream_ptr_t>(m_sink_ptr), "{0}", a_message); break;
      case sink_et::e_string : static_cast<string_ptr_t>(m_sink_ptr)->append(a_message).append("\n"); break;
      case sink_et::e_file : std::println(static_cast<file_ptr_t>(m_sink_ptr), "{0}", a_message); break;
    }
  }
  log_ct::log_ct(const span_t<sink_st> a_sinks, const configuration_st& a_cfg)
  : m_storage {a_sinks.begin(), a_sinks.end()},
    m_configuration {a_cfg} {}
  auto log_ct::configuration(this auto&& a_this) -> decltype(auto) {
    return a_this.m_configuration;
  }
  auto log_ct::insert_sink(const sink_st& a_sink) -> void {
    m_storage.emplace_back(a_sink);
  }
  auto log_ct::remove_sink(const sink_st& a_sink) -> void {
    std::erase(m_storage, a_sink);
  }
  template <event_severity_et tp_event_severity_level>
  auto log_ct::print(const string_view_t a_message, const std::source_location& a_source_location) const -> void {
    using enum configuration_st::bit_flags_et;

    constexpr auto event_severity_level = tp_event_severity_level;

    // form an estimate on how large prefixed data is in terms of character length
    // this can be used later when reserving storage for the fully formed string
    // thus avoiding unnecessary heap allocations and reallocations
    constexpr auto prefix_length = string_view_t {"[1993-01-22 - 03:40:00] event_severity_level thread 0000000000 "
                                                  "directory/directory/directory/some_file_name.cpp clever_function_name 96:81"}
                                     .length();

    using timestamp_precision_t = std::chrono::seconds;

    // data
    const auto time = std::chrono::floor<timestamp_precision_t>(chrono::current_time<system_clock_t>());
    const auto thread_id = std::this_thread::get_id();

    for (const auto& sink : m_storage) {
      // if the sink has its own configuration, use it, otherwise, use the logger's default one
      const auto& configuration = sink.m_configuration.has_value() ? sink.m_configuration.value() : m_configuration;

      // event severity level matches exactly
      const auto matching_severity_level = event_severity_level == sink.m_event_severity;
      // the sink is also subscribed to higher event severity levels
      const auto covering_higher_severity_level = event_severity_level > sink.m_event_severity and
                                                  (configuration.m_bit_mask bitand std::to_underlying(e_cover_higher_severities));

      if (matching_severity_level or covering_higher_severity_level) {
        // generate and preallocate the buffer storage
        // note: consider pulling this buffer outside of the loop and reuse it
        auto buffer = string_t {};
        buffer.reserve(prefix_length + a_message.length());

        // begin assembly
        constexpr auto prefix_delimiter = ' ';
        constexpr auto prefix_punctuator = ':';

        // timestamp
        if (configuration.m_bit_mask bitand std::to_underlying(e_print_timestamp))
          std::format_to(std::back_inserter(buffer), "[{0:%D - %T}]{1}", time, prefix_delimiter);
        // event severity level
        if (configuration.m_bit_mask bitand std::to_underlying(e_print_severity_level))
          std::format_to(std::back_inserter(buffer), "[{0}]{1}", event_severity_level_string(event_severity_level),
                         prefix_delimiter);
        // thread id
        if (configuration.m_bit_mask bitand std::to_underlying(e_print_thread_id))
          std::format_to(std::back_inserter(buffer), "[thread{2}{0}]{1}", thread_id, prefix_delimiter, prefix_punctuator);
        // source location
        if (configuration.m_bit_mask bitand std::to_underlying(e_print_source_location))
          std::format_to(std::back_inserter(buffer), "[source{5}{4}{0} at: {1}{4}{2}{5}{3}]", a_source_location.file_name(),
                         a_source_location.function_name(), a_source_location.line(), a_source_location.column(),
                         prefix_delimiter, prefix_punctuator);
        // stacktrace
        // note: add support for stacktrace printing once clang implements <stacktrace>

        // message
        buffer.append("\n").append(a_message);
        sink.print(buffer);
      }
    }
  }
  auto log_ct::information(const string_view_t a_message, const std::source_location& a_source_location) const -> void {
    print<event_severity_et::e_information>(a_message, a_source_location);
  }
  auto log_ct::warning(const string_view_t a_message, const std::source_location& a_source_location) const -> void {
    print<event_severity_et::e_warning>(a_message, a_source_location);
  }
  auto log_ct::error(const string_view_t a_message, const std::source_location& a_source_location) const -> void {
    print<event_severity_et::e_error>(a_message, a_source_location);
  }
  auto log_ct::critical(const string_view_t a_message, const std::source_location& a_source_location) const -> void {
    print<event_severity_et::e_critical>(a_message, a_source_location);
  }
}