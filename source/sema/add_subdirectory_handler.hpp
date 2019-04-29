#pragma once

#include "common/string.hpp"

#include <memory>
#include <vector>

namespace cmsl::sema {
class expression_node;
class sema_function;

class add_subdirectory_handler
{
public:
  virtual ~add_subdirectory_handler() = default;
  virtual const sema_function* handle_add_subdirectory(
    cmsl::string_view name,
    const std::vector<std::unique_ptr<expression_node>>& params) = 0;
};
}
