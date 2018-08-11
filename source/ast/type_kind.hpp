#pragma once

#include "common/string.hpp"

namespace cmsl
{
    namespace ast
    {
        enum class type_kind
        {
            k_int,
            k_real,

            k_user
        };

        type_kind type_kind_from_string(cmsl::string_view str);
    }
}
