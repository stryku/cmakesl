#pragma once

#include <stack>

namespace cmsl {
namespace facade {
class cmake_facade;
}

namespace sema {
class sema_type;
}

namespace exec {
class function_caller;
class identifiers_context;

namespace inst {
class instances_holder_interface;
}

struct expression_evaluation_context
{
  function_caller& caller;
  inst::instances_holder_interface& instances;
  identifiers_context& ids_context;
  facade::cmake_facade& cmake_facade;

  using expected_types_t =
    std::stack<std::reference_wrapper<const sema::sema_type>>;
  expected_types_t expected_types;
};
}
}
