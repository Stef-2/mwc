module;

export module mwc_chrono;

import mwc_definition;
import mwc_metaprogramming_utility;
import mwc_concept;

import std;

export namespace mwc {
  namespace chrono {
    using clock_t = std::chrono::high_resolution_clock;
    using time_point_t = std::chrono::time_point<clock_t>;
    using precision_t = std::nano;
    using integral_duration_t = std::chrono::duration<uint64_t, precision_t>;
    using floating_duration_t = std::chrono::duration<float64_t, precision_t>;
    using default_duration_t = std::chrono::nanoseconds;

    template <typename tp>
    concept duration_c = specialization_of_v<tp, std::chrono::duration> or
                         std::is_same_v<tp, integral_duration_t> or
                         std::is_same_v<tp, floating_duration_t>;

    class stopwatch_ct {
      public:
      stopwatch_ct();

      auto start() -> void;
      template <duration_c tp_duration = default_duration_t>
      [[nodiscard]] auto stop() const -> const tp_duration;

      private:
      time_point_t m_start_time;
    };

    auto initialization_time() -> const time_point_t&;
    auto current_time() -> const time_point_t;
    template <duration_c tp_duration = default_duration_t>
    auto elapsed_time() -> const tp_duration;
    template <duration_c tp_duration = default_duration_t>
    auto time_between(const time_point_t& a_first, const time_point_t& a_second)
      -> const tp_duration;

    // implementation
    const time_point_t s_initialization_time = clock_t::now();

    stopwatch_ct::stopwatch_ct() : m_start_time {} {}
    auto stopwatch_ct::start() -> void {
      m_start_time = current_time();
    }
    template <duration_c tp_duration>
    auto stopwatch_ct::stop() const -> const tp_duration {
      return {current_time() - m_start_time};
    }
    auto initialization_time() -> const time_point_t& {
      return s_initialization_time;
    }
    auto current_time() -> const time_point_t {
      return clock_t::now();
    }
    template <duration_c tp_duration = default_duration_t>
    auto elapsed_time() -> const tp_duration {
      return {current_time() - initialization_time()};
    }
    template <duration_c tp_duration = default_duration_t>
    auto time_between(const time_point_t& a_first, const time_point_t& a_second)
      -> const tp_duration {
      return std::abs(tp_duration {a_first - a_second});
    }
  }
}