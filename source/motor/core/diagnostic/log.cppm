
module;

export module mwc_log;

import mwc_definition;
import mwc_extent;
import mwc_event_severity;
import mwc_empty_type;
import mwc_metaprogramming_utility;
import mwc_concept;
import mwc_observer_ptr;
import mwc_assert;

import std;

export namespace mwc
{
  namespace diagnostic
  {
    namespace log
    {
      // possible sink types:
      // [ostream_t] -> standard output stream type
      // [string_t] -> standard string type
      // [file_t] -> standard file type
      using ostream_ptr_t = observer_ptr_t<ostream_t>;
      using string_ptr_t = observer_ptr_t<string_t>;
      using file_ptr_t = observer_ptr_t<file_t>;

      // concept modeling pointers to types which can be used as log sinks
      // possible types are: [ostream_ptr_t], [string_ptr_t], [file_ptr_t]
      template <typename t>
      concept sink_c =
        concepts::any_of_c<t, ostream_ptr_t, string_ptr_t, file_ptr_t>;

      struct configuration_st
      {
        bool m_write_timestamps = {false};
        bool m_write_severity = {false};
        bool m_write_thread_id = {false};
        bool m_write_file_name = {false};
        bool m_write_line_number = {false};
        bool m_write_function_name = {false};

        event_severity_et m_default_severity_level =
          event_severity_et::e_information;
      };

      template <sink_c tp_sink>
      struct sink_configuration_st
      {
        struct sink_st
        {
          tp_sink m_resource_ptr;
          event_severity_et m_severity_level;
        };

        using sink_t = sink_st;

        size_t m_sink_count = s_dynamic_extent;
      };

      template <bool tp_static_configuration = true,
                configuration_st tp_configuration = {},
                sink_configuration_st... tp_configs>
        requires(within_bounds<1, 3, decltype(tp_configs)...>())
      class log_ct
      {
        public:
        using sink_storage_t =
          tuple_t<extent_t<typename decltype(tp_configs)::sink_t,
                           tp_configs.m_sink_count>...>;

        //static constexpr auto s_default_severity = tp_default_severity;

        constexpr log_ct() : m_sinks {} {}

        //template <sink_c tp_sink>
        constexpr auto add_sink(const sink_c auto a_sink,
                                const event_severity_et a_severity) -> void
        {
          using sink_t = std::remove_const_t<decltype(a_sink)>;
          assert(a_sink);

          constexpr auto sink_index = storage_index<sink_t>();
          auto& sink_storage = std::get<sink_index>(m_sinks);

          constexpr auto dynamic_extent =
            instance_of_v<decltype(sink_storage), vector_t>;

          if constexpr (not dynamic_extent)
          {
            for (auto& sink : sink_storage)
              if (not sink.m_resource_ptr)
                sink = {a_sink, a_severity};
          }
          else
            m_sinks.emplace_back(a_sink, a_severity);
        }

        constexpr auto remove_sink();

        auto write_to_sinks(const string_view_t m_string,
                            const event_severity_et a_severity) const -> void
        {
          constexpr auto tuple_size = std::tuple_size_v<sink_storage_t>;

          static_for_loop<0, tuple_size>(
            [&]<size_t i>
            {
              using tuple_element_t = std::tuple_element_t<i, sink_storage_t>;
              using tuple_element_value_t = tuple_element_t::value_type;
              using underlying_sink_t = tuple_element_value_t::sink_st;

              if constexpr (std::is_same_v<underlying_sink_t, ostream_ptr_t>)
                for (const auto& sink : std::get<i>(m_sinks))
                  *sink.m_resource_ptr << m_string;
            });

          //*(std::get<0>(m_sinks)[0]) << m_string;
        }

        auto write_to_sink(const sink_c auto a_sink,
                           const string_view_t m_string) const -> void
        {
          //if constexpr (std::is_same_v<decltype(a_sink), ostream_ptr_t>)
          //std::print();
        }

        private:
        template <sink_c tp_sink>
        consteval auto storage_index() const -> size_t
        {
          constexpr auto tuple_size = std::tuple_size_v<sink_storage_t>;

          auto index = [&]<size_t i>(this const auto& self) -> size_t
          {
            using extent_value_t =
              decltype(std::tuple_element_t<i, sink_storage_t>::value_type::
                         m_resource_ptr);

            if constexpr (std::is_same_v<tp_sink, extent_value_t>)
              return i;

            if constexpr (i < tuple_size - 1)
              return self.template operator()<i + 1>();
          };

          return index.template operator()<0>();
        }

        sink_storage_t m_sinks;
        [[no_unique_address]] std::conditional_t<tp_static_configuration,
                                                 configuration_st,
                                                 empty_st> m_configuration;
      };

      void testtt()
      {
        string_t s;
        //file_t x, y, z;
        //array_t a {&x, &y, &z};

        constexpr auto asd =
          sink_configuration_st<ostream_t*> {s_dynamic_extent};
        //auto sa {&std::cout};
        //log_ct<asd> log {array_t<sink_ct<ostream_t*>, 1> {&std::cout}};
        log_ct<event_severity_et::e_information, asd> log;
        //std::get<0>(log.m_sinks)[0] = &std::cout;
        log.add_sink(&std::cout);
        /*static_assert(
          std::is_same_v<
            decltype(log)::sink_storage_t,
            tuple_t<array_t<sink_configuration_st<ostream_t*>::sink_t, 1>>>);*/
        log.write_to_sinks("Hello World!");
      }
    }
  }
}