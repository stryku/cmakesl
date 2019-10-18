#pragma once

#include "common/string.hpp"

#include <memory>
#include <variant>
#include <vector>

namespace cmsl::sema {
class expression_node;
class sema_function;

class add_subdirectory_semantic_handler
{
public:
  virtual ~add_subdirectory_semantic_handler() = default;

  struct no_script_found
  {
  };
  struct contains_old_cmake_script
  {
  };
  struct contains_cmakesl_script
  {
    const sema_function* main_function;
  };
  struct compilation_failed
  {
  };

  using add_subdirectory_result_t =
    std::variant<no_script_found, contains_old_cmake_script,
                 contains_cmakesl_script, compilation_failed>;

  virtual add_subdirectory_result_t handle_add_subdirectory(
    cmsl::string_view name,
    const std::vector<std::unique_ptr<expression_node>>& params) = 0;
};
}
