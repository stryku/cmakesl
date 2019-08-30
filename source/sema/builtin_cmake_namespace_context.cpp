#include "sema/builtin_cmake_namespace_context.hpp"

#include "lexer/token.hpp"
#include "sema/builtin_sema_function.hpp"
#include "sema/builtin_token_provider.hpp"
#include "sema/factories.hpp"
#include "sema/functions_context.hpp"
#include "sema/qualified_contextes.hpp"

#include <generated/builtin_token_providers.hpp>

namespace cmsl::sema {

builtin_cmake_namespace_context::builtin_cmake_namespace_context(
  const sema_context& parent, qualified_contextes& qualified_ctxs,
  sema_function_factory& function_factory,
  const builtin_token_provider& builtin_token_provider,
  builtin_types_accessor builtin_types)
  : sema_context_impl{ "cmake", &parent }
  , m_qualified_ctxs{ qualified_ctxs }
  , m_function_factory{ function_factory }
  , m_token_provider{ builtin_token_provider }
  , m_builtin_types{ builtin_types }
{
  auto guard =
    m_qualified_ctxs.all_qualified_ctxs_guard(m_token_provider.cmake().name());

  add_functions();
  add_variables();
}

void builtin_cmake_namespace_context::add_functions()
{
  const auto& string_type = m_builtin_types.string;
  const auto& void_type = m_builtin_types.void_;

  const auto& token_provider = m_token_provider.cmake();

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
      builtin_function_kind::cmake_fatal_error }
  };

  for (const auto& f : functions) {
    const auto& created_function = m_function_factory.create_builtin(
      *this, f.return_type, f.signature, f.kind);
    add_function(created_function);
    m_qualified_ctxs.functions.register_function(f.signature.name,
                                                 created_function);
  }
}

void builtin_cmake_namespace_context::add_variables()
{
}
}
