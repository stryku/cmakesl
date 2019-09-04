#include "sema/builtin_cmake_namespace_context.hpp"

#include "lexer/token.hpp"
#include "sema/builtin_sema_function.hpp"
#include "sema/builtin_token_provider.hpp"
#include "sema/cmake_namespace_types_accessor.hpp"
#include "sema/enum_creator.hpp"
#include "sema/factories.hpp"
#include "sema/factories_provider.hpp"
#include "sema/functions_context.hpp"
#include "sema/generic_type_creation_utils.hpp"
#include "sema/identifiers_index_provider.hpp"
#include "sema/qualified_contextes_refs.hpp"
#include "sema/type_builder.hpp"

#include <generated/builtin_token_providers.hpp>

namespace cmsl::sema {

builtin_cmake_namespace_context::builtin_cmake_namespace_context(
  const sema_context& parent, qualified_contextes_refs& qualified_ctxs,
  factories_provider& factories,
  const builtin_token_provider& builtin_token_provider,
  const builtin_types_accessor& builtin_types,
  generic_type_creation_utils& generics_creation_utils)
  : sema_context_impl{ "cmake", &parent }
  , m_qualified_ctxs{ qualified_ctxs }
  , m_factories{ factories }
  , m_builtin_tokens{ builtin_token_provider }
  , m_builtin_types{ builtin_types }
  , m_generics_creation_utils{ generics_creation_utils }
{
  auto guard = m_qualified_ctxs.all_qualified_ctxs_guard(
    m_builtin_tokens.cmake().name(), /*exported=*/false);

  add_types();
  add_functions();
  add_identifiers();
}

builtin_cmake_namespace_context::~builtin_cmake_namespace_context()
{
}

void builtin_cmake_namespace_context::add_functions()
{
  const auto& string_type = m_builtin_types.string;
  const auto& void_type = m_builtin_types.void_;

  const auto& token_provider = m_builtin_tokens.cmake();

  const auto param_token =
    lexer::make_token(lexer::token_type::identifier, "");

  const auto functions = {
    builtin_function_info{
      // void message(string)
      void_type,
      function_signature{
        token_provider.message(),
        { parameter_declaration{ string_type, param_token } } },
      builtin_function_kind::cmake_message },
    builtin_function_info{
      // void warning(string)
      void_type,
      function_signature{
        token_provider.warning(),
        { parameter_declaration{ string_type, param_token } } },
      builtin_function_kind::cmake_warning },
    builtin_function_info{
      // void error(string)
      void_type,
      function_signature{
        token_provider.error(),
        { parameter_declaration{ string_type, param_token } } },
      builtin_function_kind::cmake_error },
    builtin_function_info{
      // void fatal_error(string)
      void_type,
      function_signature{
        token_provider.fatal_error(),
        { parameter_declaration{ string_type, param_token } } },
      builtin_function_kind::cmake_fatal_error },
    builtin_function_info{
      // cxx_compiler_info get_cxx_compiler_info()
      m_types_accessor->cxx_compiler_info,
      function_signature{ token_provider.get_cxx_compiler_info() },
      builtin_function_kind::cmake_get_cxx_compiler_info }
  };

  auto factory = m_factories.function_factory();

  for (const auto& f : functions) {
    const auto& created_function =
      factory.create_builtin(*this, f.return_type, f.signature, f.kind);
    add_function(created_function);
    m_qualified_ctxs.functions.register_function(
      f.signature.name, created_function, /*exported=*/false);
  }
}

void builtin_cmake_namespace_context::add_identifiers()
{
  const auto& list_of_string_type =
    m_generics_creation_utils.list_of_strings();

  const auto identifiers = { builtin_variable_info{
    m_builtin_tokens.cmake().module_path(), list_of_string_type } };

  for (const auto& identifier : identifiers) {
    const auto identifier_index = identifiers_index_provider::get_next();
    const auto id_info = identifier_info{ identifier.type, identifier_index };

    m_qualified_ctxs.ids.register_identifier(identifier.name, id_info,
                                             /*exported=*/false);
    m_builtin_identifiers_info.emplace_back(id_info);
  }
}

const std::vector<identifier_info>&
builtin_cmake_namespace_context::builtin_identifiers_info() const
{
  return m_builtin_identifiers_info;
}

void builtin_cmake_namespace_context::add_types()
{
  const auto& cxx_compiler_id_type = add_cxx_compiler_id_type();
  auto cxx_compiler_info_manipulator =
    add_cxx_compiler_info_type(cxx_compiler_id_type);

  const auto& [cxx_compiler_info, cxx_compiler_info_ref] =
    cxx_compiler_info_manipulator.built_type();

  cmake_namespace_types_accessor accessor{
    cxx_compiler_id_type, *find_reference_for(cxx_compiler_id_type),
    cxx_compiler_info, cxx_compiler_info_ref
  };

  m_types_accessor =
    std::make_unique<cmake_namespace_types_accessor>(std::move(accessor));
}

type_builder builtin_cmake_namespace_context::add_type(lexer::token name_token)
{
  const auto name_representation =
    ast::type_representation{ ast::qualified_name{ name_token } };
  type_builder builder{ m_factories, m_qualified_ctxs.types, *this,
                        name_representation };
  builder.build_builtin_and_register_in_context();
  return builder;
}

const sema_type& builtin_cmake_namespace_context::add_cxx_compiler_id_type()
{
  static const auto token = m_builtin_tokens.cmake().cxx_compiler_id_name();
  const std::vector<lexer::token> enumerators{
    m_builtin_tokens.cmake().cxx_compiler_id_clang()
  };

  enum_creator creator{ m_factories, m_qualified_ctxs.types, *this,
                        m_builtin_types };

  const auto& type =
    creator.create(token, enumerators, sema_type::flags::builtin);

  {
    auto guard = m_qualified_ctxs.enums_ctx_guard(token, /*exported=*/false);

    unsigned value{};

    for (const auto& enumerator : enumerators) {
      const auto enum_value_index = identifiers_index_provider::get_next();
      const auto enum_value = value++;
      m_qualified_ctxs.enums.register_identifier(
        enumerator,
        enum_values_context::enum_value_info{ type, enum_value,
                                              enum_value_index },
        /*exported=*/false);
    }
  }

  return type;
}

type_builder builtin_cmake_namespace_context::add_cxx_compiler_info_type(
  const sema_type& cxx_compiler_id)
{
  static const auto token = m_builtin_tokens.cmake().cxx_compiler_info_name();
  const auto name_representation =
    ast::type_representation{ ast::qualified_name{ token } };
  type_builder builder{ m_factories, m_qualified_ctxs.types, *this,
                        name_representation };

  const auto members = { builtin_variable_info{
    m_builtin_tokens.cmake().cxx_compiler_info_id(), cxx_compiler_id } };

  for (const auto& member : members) {
    const auto id_index = identifiers_index_provider::get_next();
    member_info info{ member.name, member.type, id_index };
    builder.with_member(info);
  }

  builder.build_builtin_and_register_in_context();
  return builder;
}
const cmake_namespace_types_accessor&
builtin_cmake_namespace_context::types_accessor() const
{
  return *m_types_accessor;
}
}
