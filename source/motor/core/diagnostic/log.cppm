module;

export module mwc_log;

import mwc_definition;
import mwc_event_severity;
import mwc_sink;

import std;

export namespace mwc
{
  namespace diagnostic
  {

    template <sink_ct... sinks>
    class log_ct
    {};
  }
}