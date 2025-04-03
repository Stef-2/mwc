
module;

export module mwc_log;

import mwc_definition;
import mwc_extent;
import mwc_event_severity;
import mwc_concept;
import mwc_sink;

import std;

export namespace mwc
{
  namespace diagnostic
  {

    template <typename... sinks>
    class log_ct
    {
      public:
      //template <sinks...>
      //using drain_t = D;

      /*log_ct(
        const auto&... a_sinks)
      {}*/
      log_ct(std::tuple<sinks...>&& a_sinks) : m_sinks {a_sinks} {}

      //log_ct() {}

      private:
      std::tuple<sinks...> m_sinks;
    };
  }
}