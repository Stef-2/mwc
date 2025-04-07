
module;

export module mwc_log;

import mwc_definition;
import mwc_extent;
import mwc_event_severity;
import mwc_metaprogramming_utility;
import mwc_concept;
import mwc_sink;
import mwc_assert;

import std;

export namespace mwc
{
  namespace diagnostic
  {
    namespace log
    {
      template <drain_c tp_drain>
      struct static_configuration_st
      {
        using sink_t = tp_drain;

        size_t m_drain_count = s_dynamic_extent;
      };

      template <static_configuration_st... tp_configs>
        requires(within_bounds<1, 3, decltype(tp_configs)...>())
      class log_ct
      {
        public:
        using sink_storage_t =
          tuple_t<extent_t<typename decltype(tp_configs)::sink_t,
                           tp_configs.m_drain_count>...>;

        constexpr log_ct() {}

        //template <drain_c tp_drain>
        constexpr auto add_sink(const drain_c auto a_sink) -> void
        {
          using sink_t = std::remove_const_t<decltype(a_sink)>;
          assert(a_sink);

          constexpr auto sink_index = find_matching_extent_index<sink_t>();

          auto& sink_storage = std::get<sink_index>(m_sinks);

          constexpr auto dynamic_extent =
            instance_of_v<decltype(sink_storage), vector_t>;

          for (auto& sink : sink_storage)
            if (not sink)
              sink = a_sink;
        }

        constexpr auto remove_sink();

        auto write_to_sinks(const string_view_t m_string) -> void
        {
          *(std::get<0>(m_sinks)[0]) << m_string;
        }

        sink_storage_t m_sinks;

        //private:
        template <drain_c tp_sink>
        consteval auto find_matching_extent_index() -> const size_t
        {
          constexpr auto tuple_size = std::tuple_size_v<sink_storage_t>;

          [&]<size_t... tp>(std::index_sequence<tp...>)
          {
            auto find = [&]<size_t i>(auto&& t)
            {
              if constexpr (i > 0)
              {
                using extent_value_type_t =
                  decltype(std::get<i>(m_sinks))::value_type;

                if constexpr (std::is_same_v<tp_sink, extent_value_type_t>)

                  return i;
              }
            };

            (find.template operator()<tp>(std::get<tp>(m_sinks)), ...);
          }(std::make_index_sequence<tuple_size> {});

          return 0;
          //assert(false);
        }
      };

      /*template <static_configuration_st... tp_configs>
      log_ct(array_t<sink_ct<ostream_t*>, sizeof...(tp_configs)> a_value)
        -> log_ct<tp_configs...>;*/

      void testtt()
      {
        string_t s;
        file_t x, y, z;
        array_t a {&x, &y, &z};

        constexpr auto asd = static_configuration_st<ostream_t*> {1};
        auto sa {&std::cout};
        //log_ct<asd> log {array_t<sink_ct<ostream_t*>, 1> {&std::cout}};
        log_ct<asd> log;
        //std::get<0>(log.m_sinks)[0] = &std::cout;
        log.add_sink(&std::cout);
        static_assert(std::is_same_v<decltype(log)::sink_storage_t,
                                     tuple_t<array_t<ostream_t*, 1>>>);
      }

      template <typename tp, size_t tp_n>
      struct value_st
      {
        using value_type = tp;
        static constexpr auto s_value = tp_n;
      };
    }
  }
}