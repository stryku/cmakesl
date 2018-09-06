#pragma once

#include "exec/instance/generic_instance_value.hpp"

namespace cmsl
{

    namespace exec
    {
        namespace inst
        {
            class instance;

            enum class instance_value_type
            {
                bool_,
                int_,
                double_,
                string,
                generic
            };

            using instance_value_t = boost::variant<bool, int, double, std::string, generic_instance_value>;
        }
    }
}
