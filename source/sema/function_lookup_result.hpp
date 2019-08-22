#pragma once

#include <vector>

namespace cmsl::sema {
class sema_function;

using single_scope_function_lookup_result_t =
  std::vector<const sema_function*>;

// Is the vector needed? When function is searched for, and it's found in some
// scope, the only results that are returned are the functions from this scope.
using function_lookup_result_t =
  std::vector<single_scope_function_lookup_result_t>;
}
