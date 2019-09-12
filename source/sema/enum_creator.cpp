#include "sema/enum_creator.hpp"

#include "sema/builtin_function_kind.hpp"
#include "sema/builtin_types_accessor.hpp"
#include "sema/identifiers_index_provider.hpp"
#include "sema/type_builder.hpp"

namespace cmsl::sema {
enum_creator::enum_creator(factories_provider& factories,
                           types_context& types_ctx, sema_context& parent_ctx,
                           const builtin_types_accessor& builtin_types)
  : m_factories{ factories }
  , m_types_ctx{ types_ctx }
  , m_parent_ctx{ parent_ctx }
  , m_builtin_types{ builtin_types }
{
}

const sema_type& enum_creator::create(
  const lexer::token& name, const std::vector<lexer::token>& enumerators,
  sema_type::flags_t additional_flags)
{
  const auto name_representation =
    ast::type_representation{ ast::qualified_name{ name } };
  type_builder builder{ m_factories, m_types_ctx, m_parent_ctx,
                        name_representation,
                        sema_context::context_type ::enum_ };

  const auto& enum_type =
    builder.build_enum_and_register_in_context(enumerators, additional_flags);
  const auto& enum_ref_type = builder.built_type().reference;

  const auto& string_type = m_builtin_types.string;
  const auto& bool_type = m_builtin_types.bool_;

  const auto dummy_param_name_token =
    lexer::make_token(lexer::token_type::identifier, "");

  auto functions = {
    type_builder::builtin_function_info{
      // to_string()
      string_type,
      function_signature{
        lexer::make_token(lexer::token_type::identifier, "to_string") },
      builtin_function_kind::enum_to_string },
    type_builder::builtin_function_info{
      // operator==(enum)
      bool_type,
      function_signature{
        lexer::make_token(lexer::token_type::equalequal, "=="),
        { parameter_declaration{ enum_type, dummy_param_name_token,
                                 identifiers_index_provider::get_next() } } },
      builtin_function_kind::enum_operator_equalequal,
    },
    type_builder::builtin_function_info{
      // operator!=(enum)
      bool_type,
      function_signature{
        lexer::make_token(lexer::token_type::exclaimequal, "!="),
        { parameter_declaration{ enum_type, dummy_param_name_token,
                                 identifiers_index_provider::get_next() } } },
      builtin_function_kind::enum_operator_exclaimequal },
    type_builder::builtin_function_info{
      // operator=(enum)
      enum_ref_type,
      function_signature{
        lexer::make_token(lexer::token_type::equal, "="),
        { parameter_declaration{ enum_type, dummy_param_name_token,
                                 identifiers_index_provider::get_next() } } },
      builtin_function_kind::enum_operator_equal }
  };

  builder.with_builtin_functions(functions);

  return enum_type;
}
}
