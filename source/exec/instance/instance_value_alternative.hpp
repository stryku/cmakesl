#pragma once

namespace cmsl::exec::inst
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
                library,
                executable
            };
}
