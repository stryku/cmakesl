#pragma once

#include "sema/builtin_types_accessor.hpp"
#include "sema/import_handler.hpp"
#include "sema/sema_node.hpp"
#include "sema/sema_tree_building_context.hpp"

#include <memory>
#include <string>

namespace cmsl {
class strings_container;

namespace sema {
class add_subdirectory_handler;
class builtin_token_provider;
}
}

struct cmsl_parsed_source
{
  ~cmsl_parsed_source();

  std::string source;
  std::unique_ptr<cmsl::sema::builtin_token_provider> builtin_token_provider;
  cmsl::sema::sema_tree_building_context context;
  std::unique_ptr<cmsl::sema::add_subdirectory_handler>
    add_subdirectory_handler;
  std::unique_ptr<cmsl::sema::import_handler> imports_handler;
  std::unique_ptr<cmsl::ast::ast_node> ast_tree;
  std::unique_ptr<cmsl::sema::sema_node> sema_tree;
  std::unique_ptr<cmsl::sema::sema_context> builtin_context;
  std::unique_ptr<cmsl::strings_container> strings_container;
};
