module;

export module mwc_log;

import mwc_definition;
import mwc_configuration_type;
import mwc_event_severity;
import mwc_concept;
import mwc_observer_ptr;
import mwc_static_bi_multimap;
import mwc_empty_type;
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

      struct sink_st {
        enum class sink_et { e_ostream, e_string, e_file };

        observer_ptr_t<void> m_ptr;
        sink_et m_type;
      };

      struct static_configuration_st {

        size_t m_sink_count = {8};
        size_t m_severity_level_count =
          static_cast<size_t>(event_severity_et::end);

        /*using storage_t =
          static_bi_multimap_st<event_severity_et, tp_sink_count, sink_st,
                                tp_severity_level_count>;*/
      };

      struct dynamic_configuration_st {
        using storage_t = unordered_map_t<event_severity_et, sink_st>;
      };

      template <auto tp_cfg_specialization /* = dynamic_configuration_st {}*/>
      struct configuration_st {
        static constexpr auto static_cfg() {
          return std::is_same_v<decltype(tp_cfg_specialization), static_configuration_st>;
        }
        static constexpr auto initial_sink_count() {
          if constexpr (static_cfg())
            return tp_cfg_specialization.m_sink_count;
          else
            return std::dynamic_extent;
        }
        static constexpr auto initial_severity_level_count() {
          if constexpr (static_cfg())
            return tp_cfg_specialization.m_severity_level_count;
          else
            return 0;
        }
        using storage_t = std::conditional_t<
          static_cfg(),
          decltype(std::declval<static_bi_multimap_st<
                     event_severity_et, initial_severity_level_count(), sink_st,
                     initial_sink_count()>>()),
          dynamic_configuration_st::storage_t>;

        /*static consteval auto storage_type() {
          if constexpr (static_cfg())
            return std::declval<static_bi_multimap_st<
              event_severity_et, tp_cfg_specialization.m_severity_level_count,
              sink_st, tp_cfg_specialization.m_sink_count>>();
          else
            return dynamic_configuration_st::storage_t {};
        }*/

        static constexpr auto s_cfg_specialization = tp_cfg_specialization;
        /*using storage_t = std::conditional_t<
          static_cfg(),
          static_bi_multimap_st<event_severity_et, tp_cfg_specialization.m_severity_level_count,
                                sink_st, tp_cfg_specialization.m_sink_count>,
          dynamic_configuration_st::storage_t>;*/
        //using storage_t = decltype(storage_type());

        bool m_print_timestamps = {true};
        bool m_print_severity = {true};
        bool m_print_thread_id = {true};
        bool m_print_file_name = {true};
        bool m_print_function_name = {true};
        bool m_print_line_number = {true};
        event_severity_et m_default_severity_level = {
          event_severity_et::e_information};

        data_st<pair_t<event_severity_et, sink_st>, initial_sink_count()> m_sinks = {};
      };

      // andy sixx creamy steamy ===========================================
      template <auto tp_cfg>
      class log_ct {
        public:
        using storage_t = typename decltype(tp_cfg)::storage_t;

        /*using dynamic_storage_t = unordered_map_t<event_severity_et, sink_st>;
        using static_storage_t =
          static_bi_multimap_st<event_severity_et, tp_cfg.m_severity_level_count,
                                sink_st, tp_cfg.sink_count()>;*/

        /*using storage_t =
          std::conditional_t<tp_cfg.cfg_type() == configuration_type_et::e_static,
                             static_storage_t, dynamic_storage_t>;*/
        /*using storage_t = std::conditional_t<
          tp_cfg.cfg_type() == configuration_type_et::e_static,
          static_bi_multimap_st<event_severity_et, tp_cfg.m_severity_level_count,
                                sink_st, tp_cfg.sink_count()>,
          unordered_map_t<event_severity_et, sink_st>>;*/

        // static configuration
        constexpr log_ct()
        : m_configuration {tp_cfg},
          m_storage {tp_cfg.m_sinks.span()} {}

        // dynamic configuration
        constexpr log_ct(
          const configuration_st<dynamic_configuration_st {}>& a_cfg)
          requires std::is_same_v<decltype(tp_cfg), dynamic_configuration_st>
        : m_configuration {a_cfg},
          m_storage {a_cfg.m_sinks.begin(), a_cfg.m_sinks.end(),
                     a_cfg.initial_sink_count()} {}

        log_ct(const log_ct&) = delete("move only type");
        auto operator=(const log_ct&) -> log_ct& = delete("move only type");

        private:
        std::conditional_t<
          std::is_same_v<decltype(tp_cfg), dynamic_configuration_st>,
          configuration_st<dynamic_configuration_st {}>, decltype(tp_cfg)>
          m_configuration;
        storage_t m_storage;
      };

      /*template <auto tp_cfg = dynamic_configuration_st {}>
      log_ct(const configuration_st<tp_cfg>& a_cfg)*/

      void testtt() {
        string_t s;
        //file_t x, y, z;
        //array_t a {&x, &y, &z};

        static constexpr auto sinks =
          array_t<pair_t<event_severity_et, sink_st>, 1> {
          pair_t {event_severity_et::e_error,
                  sink_st {&std::cout, sink_st::sink_et::e_ostream}}};
        static constexpr auto scfg =
          configuration_st<static_configuration_st {1, 1}> {
          .m_sinks = data_st<pair_t<event_severity_et, sink_st>> {sinks}};
        //scfg.m_sinks = {&sinks};
        log_ct<scfg> slog {};

        /*auto dcfg = configuration_st<dynamic_configuration_st {}> {};
        log_ct<dynamic_configuration_st {}> dlog {dcfg};*/
      }
    }
  }
}