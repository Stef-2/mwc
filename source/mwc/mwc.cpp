#include "mwc/mwc.hpp"
#include "mwc/core/diagnostic/log/log.hpp"

namespace mwc {
  mwc_ct::mwc_ct(const configuration_st& a_configuration) : m_window {a_configuration.m_window_configuration} {}
  mwc_ct::~mwc_ct() {}
}