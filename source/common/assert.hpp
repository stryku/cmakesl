#pragma once

// Todo: get rid of boost
#include <boost/assert.hpp>

#define CMSL_UNREACHABLE(MSG) BOOST_ASSERT_MSG(false, MSG)
