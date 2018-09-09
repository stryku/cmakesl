#pragma once

#include "exec/instance/int_alias.hpp"
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


            using instance_value_t = boost::variant<bool,
                                                    int_t,
                                                    double,
                                                    std::string,
                                                    generic_instance_value>;

            instance_value_type get_inst_val_type(const instance_value_t& i);
        }
    }
}
