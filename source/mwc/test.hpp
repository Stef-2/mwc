module;

#include <mwc/core/diagnostic/log/log.hpp>

export module mwc_test;

// mwc_definition;
//import mwc_static_bi_multimap;

//import std;

export {
  namespace mwc {
    namespace diagnostic {
      namespace log {
        using mwc::diagnostic::log::ostream_ptr_t;
        using mwc::diagnostic::log::string_ptr_t;
        using mwc::diagnostic::log::file_ptr_t;

        using mwc::diagnostic::log::sink_st;

        using mwc::diagnostic::log::log_ct;
        void mwc::diagnostic::log::log_ct::insert_sink(const sink_st& a_sink,
                         const event_severity_et a_event_severity);
      }
    }
  }
}