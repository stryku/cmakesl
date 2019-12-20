#pragma once

#include "sema/sema_context_impl.hpp"

#include <functional>
#include <string>
#include <vector>

namespace cmsl {

namespace lexer {
class token;
}

namespace sema {

class type_builder;
class sema_type;
struct qualified_contextes_refs;
class builtin_token_provider;
class factories_provider;
struct function_signature;
struct builtin_types_accessor;
enum class builtin_function_kind;

class builtin_context_base : public sema_context_impl
{
protected:
  explicit builtin_context_base(qualified_contextes_refs& qualified_ctxs,
                                factories_provider& factories,
                                const builtin_token_provider& builtin_tokens,
                                std::string name,
                                const sema_context* parent = nullptr);

  virtual ~builtin_context_base() = default;

  struct builtin_function_info
  {
    builtin_function_kind kind{};
    const sema_type& return_type;
    lexer::token name;
    std::vector<std::reference_wrapper<const sema_type>> parameter_types;
  };

  struct builtin_variable_info
  {
    lexer::token name;
    const sema_type& type;
    std::string cmake_variable_name;
  };

  using builtin_functions_t = std::vector<builtin_function_info>;
  using token_type_t = lexer::token_type;

  type_builder add_type_and_get_builder(const lexer::token& name);

  function_signature make_function_signature(
    const builtin_function_info& info) const;

  void add_type_member_functions(type_builder& manipulator,
                                 const builtin_functions_t& functions);

  const sema_type& add_enum_type(const lexer::token& name,
                                 const std::vector<lexer::token>& enumerators,
                                 const builtin_types_accessor& builtin_types);

protected:
  qualified_contextes_refs& m_qualified_ctxs;
  factories_provider& m_factories;
  const builtin_token_provider& m_builtin_tokens;
};
}
}
