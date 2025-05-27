module;

#include <mwc/core/contract/natural_syntax.hpp>

export module mwc_log;

import mwc_definition;
import mwc_configuration_type;
import mwc_event_severity;
import mwc_concept;
import mwc_observer_ptr;
import mwc_static_bi_multimap;
import mwc_empty_type;
import mwc_metaprogramming_utility;
import mwc_data;
import mwc_optional;

import std;

export namespace mwc {
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
      concept sink_c =
        concepts::any_of_c<tp, ostream_ptr_t, string_ptr_t, file_ptr_t>;

      // type erased internal sink structure
      struct sink_st {
        enum class sink_et { e_ostream, e_string, e_file };

        constexpr auto operator==(const sink_st& a_other) const -> bool {
          return m_ptr == a_other.m_ptr and m_type == a_other.m_type;
        }
        constexpr auto print(const string_view_t a_string) const -> void {
          switch (m_type) {
            case sink_et::e_ostream :
              std::print(*static_cast<ostream_ptr_t>(m_ptr), "{0}", a_string);
              break;
            case sink_et::e_string :
              static_cast<string_ptr_t>(m_ptr)->append(a_string);
              break;
            case sink_et::e_file :
              std::print(static_cast<file_ptr_t>(m_ptr), "{0}", a_string);
              break;
          }
        }
        observer_ptr_t<void> m_ptr;
        sink_et m_type;
      };

      // static configuration
      struct static_configuration_st {
        size_t m_sink_count = {4};
        size_t m_severity_level_count =
          static_cast<size_t>(event_severity_et::end);
      };

      // dynamic configuration
      struct dynamic_configuration_st {
        size_t m_initial_sink_count = {4};
        using storage_t = unordered_multimap_t<event_severity_et, sink_st>;
      };

      // configuration, either static or dynamic
      template <auto tp_specialization = dynamic_configuration_st {}>
        requires concepts::any_of_c<decltype(tp_specialization),
                                    static_configuration_st,
                                    dynamic_configuration_st>
      struct configuration_st {
        static constexpr auto static_cfg() {
          return std::is_same_v<decltype(tp_specialization), static_configuration_st>;
        }
        static constexpr auto initial_sink_count() {
          if constexpr (static_cfg())
            return tp_specialization.m_sink_count;
          else
            return tp_specialization.m_initial_sink_count;
        }
        static constexpr auto initial_severity_level_count() {
          if constexpr (static_cfg())
            return tp_specialization.m_severity_level_count;
          else
            return 0;
        }
        // underlying log storage that will be used for the severity-sink mapping
        using storage_t = std::conditional_t<
          static_cfg(),
          decltype(std::declval<static_bi_multimap_st<event_severity_et,
                                                      initial_severity_level_count(),
                                                      sink_st,
                                                      initial_sink_count()>>()),
          dynamic_configuration_st::storage_t>;

        bool m_print_timestamps = {true};
        bool m_print_severity = {true};
        bool m_print_thread_id = {true};
        bool m_print_file_name = {true};
        bool m_print_function_name = {true};
        bool m_print_line_number = {true};
        event_severity_et m_default_severity_level = {
          event_severity_et::e_information};

        // initial sink data
        // required for static configurations
        // optional for dynamic configurations
        data_st<pair_t<event_severity_et, sink_st>, initial_sink_count()> m_sinks = {};
      };

      // log
      template <auto tp_cfg = configuration_st<> {}>
      class log_ct {
        public:
        using configuration_t = decltype(tp_cfg);
        using storage_t = typename decltype(tp_cfg)::storage_t;

        // static constructor
        constexpr log_ct()
        : m_configuration {tp_cfg},
          m_storage {tp_cfg.m_sinks.span()} {
          std::cout << "address of the first sink: "
                    << m_storage.m_values[0].m_ptr << '\n';
        }

        // dynamic constructor
        constexpr log_ct(
          const configuration_st<dynamic_configuration_st {}>& a_cfg)
          requires(not tp_cfg.static_cfg())
        : m_configuration {a_cfg},
          m_storage {a_cfg.initial_sink_count()} {
          if (not a_cfg.m_sinks.empty())
            for (const auto& [severity, sink] : a_cfg.m_sinks)
              m_storage.emplace(severity, sink);
        }

        log_ct(const log_ct&) = delete("move only type");
        auto operator=(const log_ct&) -> log_ct& = delete("move only type");

        template <typename tp_this>
        auto configuration(this tp_this&& a_this) -> configuration_t& {
          return a_this.m_configuration;
        }
        auto insert_sink(
          const sink_st& a_sink,
          const event_severity_et a_event_severity = tp_cfg.m_default_severity_level)
          pre(a_sink.m_ptr != nullptr)
            pre(a_event_severity != event_severity_et::end) -> void
          requires(not tp_cfg.static_cfg())
        {
          m_storage.emplace(a_event_severity, a_sink);
        }
        auto remove_sink(const sink_st& a_sink) pre(not m_storage.empty()) -> void
          requires(not tp_cfg.static_cfg())
        {
          for (auto& kv_pairs : m_storage)
            if (kv_pairs.second == a_sink) {
              m_storage.erase(kv_pairs.first);
              return;
            }
        }
        auto remove_severity_level(const event_severity_et a_severity)
          pre(m_storage.contains(a_severity))
            pre(a_severity != event_severity_et::end) -> void
          requires(not tp_cfg.static_cfg())
        {
          m_storage.erase(a_severity);
        }

        constexpr auto print(const string_view_t a_string,
                             const event_severity_et a_event_severity) const
          -> void {
          auto [begin, end] = m_storage.equal_range(a_event_severity);
          if constexpr (tp_cfg.static_cfg())
            std::cout << "address of the first sink: "
                      << m_storage.m_values[0].m_ptr << '\n';
          if constexpr (tp_cfg.static_cfg())
            for (; begin != end; ++begin)
              begin->print(a_string);
          else
            for (; begin != end; ++begin)
              begin->second.print(a_string);
        }

        //private:
        configuration_t m_configuration;
        storage_t m_storage;
      };

      /*void testtt() {
        static string_t s;
        //file_t x, y, z;
        //array_t a {&x, &y, &z};

        static constexpr auto sinks =
          array_t<pair_t<event_severity_et, sink_st>, 1> {pair_t {
          event_severity_et::e_error, sink_st {&s, sink_st::sink_et::e_string}}};
        std::cout << "address of std::cout: " << &std::cout << '\n';
        static constexpr auto scfg = configuration_st<static_configuration_st {1, 1}> {
          .m_sinks =
            data_st<pair_t<event_severity_et, sink_st>> {std::span(sinks)}};

        static const log_ct<scfg> slog;
        std::cout << "address of the first sink: "
                  << slog.m_storage.m_values[0].m_ptr << '\n';
        slog.print("im so static bro im an error brooo ahhh",
                   event_severity_et::e_error);
        auto dcfg = configuration_st<dynamic_configuration_st {}> {};
        static log_ct dlog {dcfg};
        dlog.insert_sink(sinks[0].second, event_severity_et::e_error);
        //dlog.remove_sink(sinks[0].second);
        //dlog.remove_severity_level(event_severity_et::e_error);
        dlog.print("hello my ERRONEOUS niggas", event_severity_et::e_error);
      }*/
    }
  }
}