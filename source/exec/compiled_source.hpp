#pragma once

#include "common/source_view.hpp"
#include "sema/builtin_types_accessor.hpp"

#include <memory>

namespace cmsl {
namespace ast {
class ast_node;
}

namespace sema {
class builtin_token_provider;
class sema_node;
class sema_function;
class builtin_sema_context;
class sema_context;
class identifiers_context;
class types_context;
class enum_values_context;
}

namespace exec {
class compiled_source
{
public:
  explicit compiled_source(std::unique_ptr<ast::ast_node> ast_tree,
                           const sema::sema_context& global_context,
                           std::unique_ptr<sema::sema_node> sema_tree,
                           source_view source,
                           sema::builtin_types_accessor builtin_types);
  ~compiled_source();

  const sema::sema_function* get_main() const;
  const sema::sema_context& get_global_context() const;

  const sema::sema_node& sema_tree() const;

  sema::builtin_types_accessor builtin_types() const;

private:
  std::unique_ptr<ast::ast_node> m_ast_tree;
  const sema::sema_context& m_global_context;
  std::unique_ptr<sema::sema_node> m_sema_tree;
  source_view m_source;
  sema::builtin_types_accessor m_builtin_types;
};
}
}
