#pragma once

#include "exec/instance/instance_value.hpp"
#include "exec/instance/instance.hpp"

// Todo: is it needed?
namespace cmsl::exec::inst
{
            class instance_value_getter
            {
            public:
                int_t int_(const instance& i) const
                {
                    return i.get_value_cref().get_int();
                }
            };
}
