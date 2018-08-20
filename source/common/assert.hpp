#pragma once

#include <boost/assert.hpp>

#define CMSL_UNREACHABLE(MSG) BOOST_ASSERT_MSG(false, MSG)
