module;

export module mwc_sink;

import mwc_definition;
import mwc_extent;
import mwc_bit;
import mwc_enum_bitwise_operators;
import mwc_observer_ptr;
import mwc_concept;
import mwc_assert;
import mwc_output_stream;

import std;

export namespace mwc
{
  namespace diagnostic
  {
    // possible sink drains:
    // [ostream_t] -> standard output stream type
    // [string_t] -> standard string type
    // [file_t] -> standard file type
    using ostream_ptr_t = observer_ptr_t<ostream_t>;
    using string_ptr_t = observer_ptr_t<string_t>;
    using file_ptr_t = observer_ptr_t<file_t>;

    // concept modeling pointers to types which can be used as sink drains
    // possible types are: [ostream_ptr_t], [string_ptr_t], [file_ptr_t]
    template <typename t>
    concept drain_c =
      concepts::any_of_c<t, ostream_ptr_t, string_ptr_t, file_ptr_t>;

    // type which abstracts writing character strings to an output target (drain)
    // possible drains are output streams (console / terminal), string objects in memory or files
    // sink_ct has no ownership of any drain passed to it
    template <drain_c tp_drain>
    class sink_ct
    {
      public:
      using drain_t = tp_drain;

      // underlying drain storage depends on the [m_drain_count] template parameter
      // if [m_drain_count] is not [s_dynamic_extent], the storage is [array_t<drain_t, m_drain_count>]
      // if [m_drain_count] is [s_dynamic_extent], the storage is [vector_t<drain_t>]
      //using drain_storage_t = extent_t<drain_t, tp_cfg.m_drain_count>;

      sink_ct();
      sink_ct(const drain_t a_drain);

      // disable copy constructor and assignment operator
      sink_ct(const sink_ct&) = delete("move only type");
      sink_ct& operator=(const sink_ct&) = delete("move only type");

      auto drain(const drain_t a_drain) -> void;
      auto drain() const -> const drain_t;
      auto write_to_drain(const string_view_t a_string) const -> void;

      private:
      drain_t m_drain;
    };

    // explicit deduction guides
    /*template <drain_c d>
      sink_ct(vector_t<d> a_value) -> sink_ct<d, s_dynamic_extent>;
      template <drain_c d, size_t n>
      sink_ct(array_t<d, n> a_value) -> sink_ct<d, n>;
      template <drain_c d>
      sink_ct(d a_value) -> sink_ct<d, 1>;*/

    // ================================
    // implementation
    // ================================
    /*template <auto>
      sink_ct<static_configuration_st>::sink_ct(const auto& a_drains)
      : m_drains {a_drains}
      {
        for (const auto drain : m_drains)
          assert(drain);
      }*/
    /*
      template <static_configuration_st>
      auto sink_ct<static_configuration_st>::add_drain(const drain_t a_drain)
        -> void
        requires(std::is_same_v<drain_storage_t, vector_t<drain_t>>)
      {
        assert(a_drain);
        m_drains.emplace_back(a_drain);
      }*/
    /*
      template <drain_c drain_t, size_t drain_count>
      auto sink_ct<drain_t, drain_count>::remove_drain(const drain_t a_drain)
        -> void
        requires(std::is_same_v<drain_storage_t, vector_t<drain_t>>)
      {
        assert(a_drain);
        std::erase(m_drains, a_drain);
      }*/
    /*
      template <drain_c drain_t, size_t drain_count>
      auto sink_ct<drain_t, drain_count>::write_to_drains(
        const string_view_t a_string) -> void
      {
        // note: consider adding a a special constexpr check for [stream_count == 1]
        // in case the compiler doesn't optimize away the for_each loop

        if constexpr (std::is_same_v<drain_t, ostream_ptr_t>)
          for (const auto drain : m_drains)
            std::print(*drain, "{0}", a_string);

        if constexpr (std::is_same_v<drain_t, file_ptr_t>)
          for (const auto drain : m_drains)
            std::print(drain, "{0}", a_string);

        if constexpr (std::is_same_v<drain_t, string_ptr_t>)
          for (const auto drain : m_drains)
            drain->append(a_string);
      }
*/
    template <drain_c tp_drain>
    sink_ct<tp_drain>::sink_ct() : m_drain {nullptr}
    {}

    template <drain_c tp_drain>
    sink_ct<tp_drain>::sink_ct(const drain_t a_drain) : m_drain {a_drain}
    {
      assert(m_drain);
    }

    template <drain_c tp_drain>
    auto sink_ct<tp_drain>::drain(const drain_t a_drain) -> void
    {
      assert(a_drain);
      m_drain = a_drain;
    }

    template <drain_c tp_drain>
    auto sink_ct<tp_drain>::drain() const -> const drain_t
    {
      return m_drain;
    }

    template <drain_c tp_drain>
    auto sink_ct<tp_drain>::write_to_drain(const string_view_t a_string) const
      -> void
    {
      // note: consider adding a a special constexpr check for [stream_count == 1]
      // in case the compiler doesn't optimize away the for_each loop

      if constexpr (std::is_same_v<drain_t, ostream_ptr_t>)
        std::print(*m_drain, "{0}", a_string);

      if constexpr (std::is_same_v<drain_t, file_ptr_t>)
        std::print(m_drain, "{0}", a_string);

      if constexpr (std::is_same_v<drain_t, string_ptr_t>)
        m_drain->append(a_string);
    }
  }
}