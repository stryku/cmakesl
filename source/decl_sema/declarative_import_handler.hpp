#pragma once

#include "sema/qualified_contextes.hpp"

#include <optional>

namespace cmsl::decl_sema {
class component_declaration_node;

class declarative_import_handler
{
public:
  virtual ~declarative_import_handler() = default;

  struct result
  {
    sema::qualified_contextes qualified_contexts;
    cmsl::string_view_map<const decl_sema::component_declaration_node*>
      component_declarations;
  };

  virtual std::optional<result> handle_declarative_import(
    cmsl::string_view path) = 0;
};
}
