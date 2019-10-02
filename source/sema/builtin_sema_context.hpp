#pragma once

#include "sema/builtin_function_kind.hpp"
#include "sema/builtin_types_accessor.hpp"
#include "sema/function_signature.hpp"
#include "sema/identifier_info.hpp"
#include "sema/sema_context_impl.hpp"

namespace cmsl {
namespace errors {
class errors_observer;
}

namespace exec::inst {
class instance;
}

namespace sema {
class builtin_token_provider;
class factories_provider;
enum class builtin_function_kind;
class type_builder;
class qualified_contextes_refs;
class generic_type_creation_utils;
class builtin_cmake_namespace_context;

class builtin_sema_context : public sema_context_impl
{
public:
  explicit builtin_sema_context(
    factories_provider& factories_provider,
    errors::errors_observer& errors_observer,
    const builtin_token_provider& builtin_token_provider,
    qualified_contextes_refs& qualified_ctxs);
  ~builtin_sema_context() override;

  builtin_types_accessor builtin_types() const;

  std::vector<builtin_identifier_info> builtin_identifiers_info() const;

private:
  using token_type_t = lexer::token_type;

  struct builtin_function_info
  {
    const sema_type& return_type;
    function_signature signature;
    builtin_function_kind kind;
  };

  // Types are added in two phases:
  // 1. Adding types with its members to sema context.
  // 2. Adding member functions to already created members.
  // It's done this way because member function can refer to type that has not
  // been created yet. E.g. bool has method `string to_string()` which returns
  // `string`, and string has method `bool empty()` which returns `bool`
  void add_types();
  void add_functions();

  void add_cmake_namespace_context();

  template <typename Functions>
  void add_type_member_functions(type_builder& manipulator,
                                 Functions&& functions);

  type_builder add_type(lexer::token name_token);

  type_builder add_bool_type();
  void add_bool_member_functions(type_builder& bool_manipulator);
  type_builder add_int_type();
  void add_int_member_functions(type_builder& int_manipulator);
  type_builder add_double_type();
  void add_double_member_functions(type_builder& double_manipulator);
  type_builder add_string_type();
  void add_string_member_functions(type_builder& string_manipulator);
  type_builder add_void_type();

  template <unsigned N>
  lexer::token make_token(token_type_t token_type, const char (&tok)[N]);

  template <unsigned N>
  lexer::token make_id_token(const char (&tok)[N]);

private:
  factories_provider& m_factories;
  errors::errors_observer& m_errors_observer;
  const builtin_token_provider& m_builtin_token_provider;
  qualified_contextes_refs& m_qualified_ctxs;

  std::unique_ptr<builtin_types_accessor> m_builtin_types;
  std::unique_ptr<generic_type_creation_utils> m_generics_creation_utils;
  std::unique_ptr<builtin_cmake_namespace_context> m_cmake_namespace_context;
};
}
}