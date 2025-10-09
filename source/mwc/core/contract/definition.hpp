#pragma once

#include "mwc/core/diagnostic/assert.hpp"

#define pre(a_expression)
#define post(a_expression)
#define contract_assert(a_expression) mwc::sanity_check(a_expression)