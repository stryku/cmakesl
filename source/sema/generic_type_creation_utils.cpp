#include "sema/generic_type_creation_utils.hpp"
#include "lexer/token.hpp"
#include "sema/builtin_types_accessor.hpp"
#include "sema/factories.hpp"
#include "sema/factories_provider.hpp"

namespace cmsl::sema {

generic_type_creation_utils::generic_type_creation_utils(
  sema_context& generic_types_context, factories_provider& factories,
  errors::errors_observer& errors_observer,
  const builtin_token_provider& builtin_token_provider,
  const builtin_types_accessor& builtin_types, types_context& types_ctx)
  : m_generic_types_context{ generic_types_context }
  , m_factories{ factories }
  , m_errors_observer{ errors_observer }
  , m_builtin_token_provider{ builtin_token_provider }
  , m_builtin_types{ builtin_types }
  , m_types_ctx{ types_ctx }
{
}

const sema_type& generic_type_creation_utils::list_of_strings()
{
  const auto string_token = make_token(lexer::token_type::kw_string, "string");
  const auto string_type_representation =
    ast::type_representation{ ast::qualified_name{ string_token } };

  auto generic_name = ast::type_representation::generic_type_name{
    { make_token(lexer::token_type::kw_list, "list"),
      make_token(lexer::token_type::less, "<"), string_token,
      make_token(lexer::token_type::greater, ">") },
    { string_type_representation }
  };
  const auto sources_list_type_name_representation =
    ast::type_representation{ generic_name };
  return get_or_create_generic_type(sources_list_type_name_representation);
}

const sema_type& generic_type_creation_utils::get_or_create_generic_type(
  const ast::type_representation& type_representation)
{
  if (const auto found =
        m_generic_types_context.find_type(type_representation)) {
    return *found;
  }

  auto factory = sema_generic_type_factory{ m_generic_types_context,
                                            m_generic_types_context,
                                            m_factories,
                                            m_errors_observer,
                                            m_builtin_token_provider,
                                            m_builtin_types,
                                            m_types_ctx };
  return *factory.create_generic(type_representation);
}
}
