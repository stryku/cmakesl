#pragma once

#include "common/string.hpp"

#include <variant>

namespace cmsl::sema {
class sema_function;

class add_declarative_file_semantic_handler
{
public:
  virtual ~add_declarative_file_semantic_handler() = default;

  struct no_script_found
  {
  };
  struct contains_declarative_cmakesl_script
  {
    const sema_function* target_creation_function;
  };
  struct compilation_failed
  {
  };

  using add_declarative_file_result_t =
    std::variant<no_script_found, contains_declarative_cmakesl_script,
                 compilation_failed>;

  virtual add_declarative_file_result_t handle_add_declarative_file(
    cmsl::string_view name) = 0;
};
}
