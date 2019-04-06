#pragma once

#include <vector>

namespace cmsl::sema
{
        class sema_function;

        using single_scope_function_lookup_result_t = std::vector<const sema_function*>;
        using function_lookup_result_t = std::vector<single_scope_function_lookup_result_t>;
}
