#pragma once

#include "exec/instance/int_alias.hpp"
#include "exec/instance/instance_value_variant.hpp"

// Todo: is it needed?
namespace cmsl::exec::inst
{
            enum class instance_value_type
            {
                bool_,
                int_,
                double_,
                string,

                generic
            };

            using instance_value_t = instance_value_variant;

            instance_value_type get_inst_val_type(const instance_value_t& i);
}
