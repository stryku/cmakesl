#pragma once

#include <cassert>

#define CMSL_ASSERT(EXPR) assert((EXPR))
#define CMSL_ASSERT_MSG(EXPR, MSG) assert((EXPR) && (MSG))
#define CMSL_UNREACHABLE(MSG) assert(false && (MSG))
