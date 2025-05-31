#pragma once

//#include <mwc/core/contract/natural_syntax.hpp>
#include "mwc/core/container/data.hpp"
#include "mwc/core/container/static_bi_multimap.hpp"
#include "mwc/core/definition/definition.hpp"
//export module mwc_log;

//import mwc_definition;
//import mwc_configuration_type;
import mwc_event_severity;
import mwc_concept;
import mwc_observer_ptr;
//import mwc_static_bi_multimap;
//import mwc_empty_type;
import mwc_metaprogramming_utility;
//import mwc_data;

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
      concept sink_c =
        concepts::any_of_c<tp, ostream_ptr_t, string_ptr_t, file_ptr_t>;

      // type erased internal sink structure
      struct sink_st {
        enum class sink_et { e_ostream, e_string, e_file };

        constexpr auto operator==(const sink_st& a_other) const -> bool {
          return m_ptr == a_other.m_ptr and m_type == a_other.m_type;
        }
        auto print(const string_view_t a_string) const -> void {
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

      // log
      class log_ct {
        public:
        struct configuration_st {
          bool m_print_thread_id;
          bool m_print_file_name;
          bool m_print_function_name;
          bool m_print_line_number;
          event_severity_et m_default_severity_level;

          static constexpr auto default_cfg() -> configuration_st {
            return configuration_st {.m_print_thread_id = true,
                                     .m_print_file_name = true,
                                     .m_print_function_name = true,
                                     .m_print_line_number = true,
                                     .m_default_severity_level =
                                       event_severity_et::e_information};
          }
        };
        using storage_t = std::unordered_multimap<event_severity_et, sink_st>;
        // dynamic constructor
        log_ct(const configuration_st& a_cfg = configuration_st::default_cfg())
        : m_configuration {a_cfg},
          m_storage {} {
          /*if (not a_cfg.m_sinks.empty())
            for (const auto& [severity, sink] : a_cfg.m_sinks)
              m_storage.emplace(severity, sink);*/
        }

        log_ct(const log_ct&) = delete("move only type");
        auto operator=(const log_ct&) -> log_ct& = delete("move only type");

        template <typename tp_this>
        auto configuration(this tp_this&& a_this) -> configuration_st& {
          return a_this.m_configuration;
        }
        auto insert_sink(const sink_st& a_sink,
                         const event_severity_et a_event_severity)
          -> void pre(a_sink.m_ptr != nullptr and
                      a_event_severity != event_severity_et::end)

        {
          m_storage.emplace(a_event_severity, a_sink);
        }
        auto remove_sink(const sink_st& a_sink)
          -> void pre(not m_storage.empty()) {
          for (auto& kv_pairs : m_storage)
            if (kv_pairs.second == a_sink) {
              m_storage.erase(kv_pairs.first);
              return;
            }
        }
        auto remove_severity_level(const event_severity_et a_severity)
          -> void pre(m_storage.contains(a_severity))
            pre(a_severity != event_severity_et::end) {
          m_storage.erase(a_severity);
        }

        auto print(const string_view_t a_string,
                   const event_severity_et a_event_severity) const -> void {
          auto [begin, end] = m_storage.equal_range(a_event_severity);
          for (; begin != end; ++begin)
            begin->second.print(a_string);
        }

        //private:
        configuration_st m_configuration;
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
        static constexpr auto scfg =
          configuration_st<static_configuration_st {1, 1}> {
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