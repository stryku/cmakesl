#pragma once

#include "common/string.hpp"

namespace cmsl::ast
{
        // Todo: Move to sema namespace
        // Todo: is it needed?
        enum class type_kind
        {
            k_void,
            k_int,
            k_double,
            k_bool,
            k_string,
            k_list,
            k_version,
            k_project,

            k_user
        };

        type_kind type_kind_from_string(cmsl::string_view str);
}
