#pragma once

#include <boost/variant.hpp>

#include <string>

namespace cmsl
{
    namespace exec
    {
        namespace inst
        {
            using instance_value_t = boost::variant<bool, int, double, std::string>;
        }
    }
}
