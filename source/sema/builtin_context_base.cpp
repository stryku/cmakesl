#include "sema/builtin_context_base.hpp"
#include "sema/enum_creator.hpp"
#include "sema/identifiers_index_provider.hpp"
#include "sema/qualified_contextes_refs.hpp"
#include "sema/type_builder.hpp"

namespace {
const auto param_token =
  cmsl::lexer::make_token(cmsl::lexer::token_type::identifier, "");
}

namespace cmsl::sema {
builtin_context_base::builtin_context_base(
  qualified_contextes_refs& qualified_ctxs, factories_provider& factories,
  const builtin_token_provider& builtin_tokens, std::string name,
  const sema_context* parent)
  : sema_context_impl{ std::move(name), parent }
  , m_qualified_ctxs{ qualified_ctxs }
  , m_factories{ factories }
  , m_builtin_tokens{ builtin_tokens }
{
}

type_builder builtin_context_base::add_type_and_get_builder(
  const lexer::token& name)
{
  const auto name_representation =
    ast::type_representation{ ast::qualified_name{ name } };
  type_builder builder{ m_factories, m_qualified_ctxs.types, *this,
                        name_representation };
  builder.build_builtin_and_register_in_context();
  return builder;
}

function_signature builtin_context_base::make_function_signature(
  const builtin_context_base::builtin_function_info& info) const
{
  std::vector<parameter_declaration> param_declarations;

  param_declarations.reserve(info.parameter_types.size());

  std::transform(std::cbegin(info.parameter_types),
                 std::cend(info.parameter_types),
                 std::back_inserter(param_declarations), [](const auto& type) {
                   return parameter_declaration{ type, param_token };
                 });

  return function_signature{ info.name, std::move(param_declarations) };
}

void builtin_context_base::add_type_member_functions(
  type_builder& manipulator, const builtin_functions_t& functions)
{
  for (const auto& function : functions) {
    auto signature = make_function_signature(function);
    manipulator.with_builtin_function(function.return_type,
                                      std::move(signature), function.kind);
  }
}

const sema_type& builtin_context_base::add_enum_type(
  const lexer::token& name, const std::vector<lexer::token>& enumerators,
  const builtin_types_accessor& builtin_types)
{
  enum_creator creator{ m_factories, m_qualified_ctxs.types, *this,
                        builtin_types };

  const auto& type =
    creator.create(name, enumerators, sema_type::flags::builtin);

  {
    auto guard = m_qualified_ctxs.enums_ctx_guard(name, /*exported=*/false);

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

}