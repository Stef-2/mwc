#include "mwc/mwc.hpp"
#include "mwc/core/diagnostic/log/log.hpp"

namespace mwc {
  mwc_ct::mwc_ct(const configuration_st& a_configuration)
  : m_window {a_configuration.m_window_configuration},
    m_configuration {a_configuration} {}
  mwc_ct::~mwc_ct() {}
}