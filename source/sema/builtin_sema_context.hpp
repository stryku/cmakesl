#pragma once

#include "sema/builtin_context_base.hpp"
#include "sema/builtin_function_kind.hpp"
#include "sema/builtin_types_accessor.hpp"
#include "sema/function_signature.hpp"
#include "sema/identifier_info.hpp"

namespace cmsl {
namespace errors {
class errors_observer;
}

namespace exec::inst {
class instance;
}

namespace sema {
class builtin_cmake_namespace_context;
class builtin_token_provider;
class factories_provider;
class generic_type_creation_utils;
class type_builder;
enum class builtin_function_kind;
struct qualified_contextes_refs;

class builtin_sema_context : public builtin_context_base
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

  generic_type_creation_utils& generic_creation_utils();

private:
  using token_type_t = lexer::token_type;

  // Types are added in two phases:
  // 1. Adding types with its members to sema context.
  // 2. Adding member functions to already created members.
  // It's done this way because member function can refer to type that has not
  // been created yet. E.g. bool has method `string to_string()` which returns
  // `string`, and string has method `bool empty()` which returns `bool`
  void add_types();
  void add_functions();

  void add_cmake_namespace_context();

  type_builder add_bool_type();
  void add_bool_member_functions(type_builder& bool_manipulator);
  type_builder add_int_type();
  void add_int_member_functions(type_builder& int_manipulator);
  type_builder add_double_type();
  void add_double_member_functions(type_builder& double_manipulator);
  type_builder add_string_type();
  void add_string_member_functions(type_builder& string_manipulator);
  type_builder add_void_type();

private:
  errors::errors_observer& m_errors_observer;

  std::unique_ptr<builtin_types_accessor> m_builtin_types;
  std::unique_ptr<generic_type_creation_utils> m_generics_creation_utils;
  std::unique_ptr<builtin_cmake_namespace_context> m_cmake_namespace_context;
};
}
}