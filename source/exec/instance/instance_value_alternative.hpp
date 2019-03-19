#pragma once

namespace cmsl
{
    namespace exec
    {
        namespace inst
        {
            enum class instance_value_alternative
            {
                bool_,
                int_,
                double_,
                string,
                version,
                list,
                project,
                generic
            };
        }
    }
}
