module;

export module mwc_log;

import mwc_definition;
import mwc_extent;
import mwc_event_severity;
import mwc_empty_type;
import mwc_metaprogramming_utility;
import mwc_concept;
import mwc_observer_ptr;
import mwc_static_bi_multimap;
import mwc_configuration_type;

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
      template <typename t>
      concept sink_c =
        concepts::any_of_c<t, ostream_ptr_t, string_ptr_t, file_ptr_t>;

      struct sink_st {
        enum class sink_et { e_ostream, e_string, e_file };

        observer_ptr_t<void> m_ptr;
        sink_et m_type;
      };

      struct static_configuration_st {
        static constexpr size_t m_ostream_sink_count = {4};
        static constexpr size_t m_string_sink_count = {4};
        static constexpr size_t m_file_sink_count = {4};
        size_t m_severity_level_count =
          static_cast<size_t>(event_severity_et::end);
      };

      struct dynamic_configuration_st {
        static constexpr size_t m_initial_sink_count = {4};
      };

      template <configuration_type_et tp_cfg_type = configuration_type_et::e_static>
      struct configuration_st
      : public std::conditional_t<tp_cfg_type == configuration_type_et::e_static,
                                  static_configuration_st, dynamic_configuration_st> {
        static constexpr auto cfg_type() { return tp_cfg_type; }

        static constexpr auto sink_count() {
          return cfg_type() == configuration_type_et::e_static
                 ? static_configuration_st::m_ostream_sink_count +
                     static_configuration_st::m_string_sink_count +
                     static_configuration_st::m_file_sink_count
                 : dynamic_configuration_st::m_initial_sink_count;
        }

        bool m_print_timestamps = {true};
        bool m_print_severity = {true};
        bool m_print_thread_id = {true};
        bool m_print_file_name = {true};
        bool m_print_function_name = {true};
        bool m_print_line_number = {true};

        event_severity_et m_default_severity_level = {
          event_severity_et::e_information};

        array_t<sink_st, sink_count()> m_sinks = {};
      };

      // andy sixx creamy steamy
      /*template <configuration_st tp_cfg = {}>
      class log_ct {
        public:
        using dynamic_storage_t = unordered_map_t<event_severity_et, sink_st>;
        using static_storage_t =
          static_bi_multimap_st<event_severity_et, tp_cfg.m_severity_level_count,
                                sink_st, tp_cfg.sink_count()>;

        using storage_t =
          std::conditional_t<tp_cfg.cfg_type() == configuration_type_et::e_static,
                             static_storage_t, dynamic_storage_t>;

        // static configuration
        constexpr log_ct()
        : m_configuration {tp_cfg},
          m_storage {[] {
            pair_t<event_severity_et, sink_st> array[tp_cfg.sink_count()];
            for (auto i = 0uz; i < tp_cfg.sink_count(); ++i)
              array[i] = {tp_cfg.m_default_severity_level, tp_cfg.m_sinks[i]};
            return array;
          }()} {}

        // dynamic configuration
        constexpr log_ct(
          const configuration_st<configuration_type_et::e_dynamic>& a_cfg)
          requires(tp_cfg.s_cfg_type == configuration_type_et::e_dynamic)
        : m_configuration {a_cfg},
          m_storage {a_cfg.m_default_severity_level, a_cfg.m_sinks} {}

        log_ct(const log_ct&) = delete("move only type");
        auto operator=(const log_ct&) -> log_ct& = delete("move only type");

        private:
        configuration_st<tp_cfg.cfg_type()> m_configuration;
        storage_t m_storage;
      };*/

      void testtt() {
        string_t s;
        //file_t x, y, z;
        //array_t a {&x, &y, &z};

        constexpr auto scfg = configuration_st {};
        //log_ct<scfg> log {};
      }
    }
  }
}