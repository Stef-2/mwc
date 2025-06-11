#pragma once

import mwc_definition;
import mwc_event_severity;
import mwc_contract_assertion;
import mwc_concept;
import mwc_set_bit;
import mwc_bit_mask;
import mwc_enum_bitwise_operators;
import mwc_observer_ptr;

import std;

namespace mwc {
  namespace diagnostic {
    namespace log {
      // possible sink types:
      // [ostream_t] -> standard output stream type
      // [string_t] -> standard string type
      // [file_t] -> standard file type
      using ostream_ptr_t = observer_ptr_t<ostream_t>;
      using string_ptr_t = observer_ptr_t<string_t>;
      using file_ptr_t = observer_ptr_t<file_t>;

      // concept modeling types which can be used as log sinks
      template <typename tp>
      concept sink_c = concepts::any_of_c<tp, ostream_ptr_t, string_ptr_t, file_ptr_t>;

      // configuration for individual sinks and the logger
      struct configuration_st {
        // default severity level to which sinks and logs subscribe to
        static constexpr auto s_default_severity_level = event_severity_et::e_information;

        // note: add support for stacktrace printing once clang implements <stacktrace>
        enum class bit_flags_et : uint8_t {
          e_print_timestamp = utility::set_bit<1>(),
          e_print_severity_level = utility::set_bit<2>(),
          e_print_thread_id = utility::set_bit<3>(),
          e_print_source_location = utility::set_bit<4>(),
          e_print_stacktrace = utility::set_bit<5>(),
          e_cover_higher_severities = utility::set_bit<6>()
        };

        // note: consider using std::bitset for this bit mask
        bit_mask_t<bit_flags_et> m_bit_mask = std::numeric_limits<bit_mask_t<bit_flags_et>>::max();
      };

      // type erased internal sink structure
      struct sink_st {
        // note: consider using std::variant instead of type erasure
        enum class sink_et : uint8_t { e_ostream, e_string, e_file };

        constexpr sink_st(const sink_c auto a_sink,
                          optional_t<event_severity_et> a_event_severity = configuration_st::s_default_severity_level,
                          optional_t<configuration_st> a_cfg = {}) pre(a_sink != nullptr)
          pre(a_event_severity != event_severity_et::end);
        constexpr auto operator==(const sink_st& a_other) const -> bool;
        constexpr auto operator==(const sink_c auto a_sink) const -> bool pre(a_sink != nullptr);
        auto print(const string_view_t a_string) const -> void;

        observer_ptr_t<void> m_sink_ptr;
        sink_et m_type;
        optional_t<event_severity_et> m_event_severity;
        optional_t<configuration_st> m_configuration;
      };

      // logging subsystem
      class log_ct {
        public:
        using storage_t = vector_t<sink_st>;

        log_ct(const span_t<sink_st> a_sinks = {}, const configuration_st& a_cfg = {});
        log_ct(const log_ct&) = delete("move only type");
        auto operator=(const log_ct&) -> log_ct& = delete("move only type");

        template <typename tp_this>
        auto storage(this tp_this&& a_this) -> decltype(auto);
        auto configuration(this auto&& a_this) -> configuration_st&;
        auto insert_sink(const sink_st& a_sink) -> void pre(a_sink.m_sink_ptr != nullptr);
        auto remove_sink(const sink_st& a_sink) -> void pre(not m_storage.empty());
        auto information(const string_view_t a_message,
                         const std::source_location& a_source_location = std::source_location::current()) const
          -> void pre(contract::validate_storage(a_message));
        auto warning(const string_view_t a_message,
                     const std::source_location& a_source_location = std::source_location::current()) const
          -> void pre(contract::validate_storage(a_message));
        auto error(const string_view_t a_message,
                   const std::source_location& a_source_location = std::source_location::current()) const
          -> void pre(contract::validate_storage(a_message));
        auto critical(const string_view_t a_message,
                      const std::source_location& a_source_location = std::source_location::current()) const
          -> void pre(contract::validate_storage(a_message));

        private:
        template <event_severity_et tp_event_severity_level>
        auto print(const string_view_t a_message, const std::source_location& a_source_location) const
          -> void pre(tp_event_severity_level != event_severity_et::end);

        storage_t m_storage;
        // configuration to be used if a sink has no configuration of its own
        configuration_st m_configuration;
      };

      // implementation
      constexpr sink_st::sink_st(const sink_c auto a_sink, optional_t<event_severity_et> a_event_severity,
                                 optional_t<configuration_st> a_cfg)
      : m_sink_ptr {a_sink},
        m_type {sink_et::e_ostream},
        m_event_severity {a_event_severity},
        m_configuration {a_cfg} {
        using sink_t = std::remove_cvref_t<decltype(a_sink)>;

        if constexpr (std::is_same_v<sink_t, ostream_ptr_t>)
          m_type = sink_et::e_ostream;
        else if constexpr (std::is_same_v<sink_t, string_ptr_t>)
          m_type = sink_et::e_string;
        else if constexpr (std::is_same_v<sink_t, file_ptr_t>)
          m_type = sink_et::e_file;
        else
          contract_assert(false);
      }
      template <typename tp_this>
      auto log_ct::storage(this tp_this&& a_this) -> decltype(auto) {
        return a_this.m_storage;
      }
    }
  }
}