#pragma once

#include "exec/instance/instance_value.hpp"
#include "exec/instance/instance.hpp"

namespace cmsl
{
    namespace exec
    {
        namespace inst
        {
            class instance_value_getter
            {
            public:
                int_t int_(const instance& i) const
                {
                    const auto& val = i.get_value_cref();
                    return boost::get<int_t>(val);
                }
            };
        }
    }
}
