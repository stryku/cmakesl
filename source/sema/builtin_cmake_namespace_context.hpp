#pragma once

#include "sema/builtin_function_kind.hpp"
#include "sema/builtin_types_accessor.hpp"
#include "sema/function_signature.hpp"
#include "sema/identifier_info.hpp"
#include "sema/sema_context_impl.hpp"

namespace cmsl::sema {
class builtin_token_provider;
class factories_provider;
class qualified_contextes_refs;
class generic_type_creation_utils;
struct cmake_namespace_types_accessor;
class type_builder;

class builtin_cmake_namespace_context : public sema_context_impl
{
public:
  explicit builtin_cmake_namespace_context(
    const sema_context& parent, qualified_contextes_refs& qualified_ctxs,
    factories_provider& factories,
    const builtin_token_provider& builtin_token_provider,
    const builtin_types_accessor& builtin_types,
    generic_type_creation_utils& generics_creation_utils);
  virtual ~builtin_cmake_namespace_context();

  const std::vector<builtin_identifier_info>& builtin_identifiers_info() const;
  const cmake_namespace_types_accessor& types_accessor() const;

private:
  using token_type_t = lexer::token_type;
  struct builtin_function_info
  {
    const sema_type& return_type;
    function_signature signature;
    builtin_function_kind kind{};
  };
  struct builtin_variable_info
  {
    const lexer::token& name;
    const sema_type& type;
    std::string cmake_variable_name;
  };

  void add_types();
  void add_functions();
  void add_identifiers();

  type_builder add_type(lexer::token name_token);

  const sema_type& add_cxx_compiler_id_type();
  const sema_type& add_cxx_standard_value_type();
  const sema_type& add_visibility_type();
  type_builder add_cxx_compiler_info_type(const sema_type& cxx_compiler_id);

  const sema_type& add_system_id_type();
  const sema_type& add_system_info_type(const sema_type& system_id);

  type_builder add_version_type();
  void add_version_member_functions(type_builder& string_manipulator);
  type_builder add_library_type();
  void add_library_member_functions(type_builder& project_manipulator);
  type_builder add_executable_type();
  void add_executable_member_functions(type_builder& project_manipulator);
  type_builder add_project_type();
  void add_project_member_functions(type_builder& project_manipulator);
  type_builder add_option_type();
  void add_option_member_functions(type_builder& project_manipulator);

  const sema_type& get_or_create_generic_type(
    const ast::type_representation& type_representation);

  template <typename Functions>
  void add_type_member_functions(type_builder& manipulator,
                                 Functions&& functions);

private:
  qualified_contextes_refs& m_qualified_ctxs;
  factories_provider& m_factories;
  const builtin_token_provider& m_builtin_tokens;
  const builtin_types_accessor& m_builtin_types;
  generic_type_creation_utils& m_generics_creation_utils;

  std::vector<builtin_identifier_info> m_builtin_identifiers_info;
  std::unique_ptr<cmake_namespace_types_accessor> m_types_accessor;
};
}
