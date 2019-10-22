#include "ast/type_parser.hpp"

#include "ast/parse_errors_observer.hpp"
#include "ast/type_parsing_result.hpp"
#include "ast/type_representation.hpp"
#include "common/algorithm.hpp"
#include "type_parser.hpp"

namespace cmsl::ast {
type_parser::type_parser(parse_errors_observer& err_observer,
                         parser_utils::token_it current,
                         parser_utils::token_it end)
  : parser_utils{ err_observer, current, end }
  , m_err_observer{ err_observer }
{
}

type_parsing_result type_parser::type()
{
  auto got_type = parse_type();
  auto stopped_at = current_iterator();
  return type_parsing_result{ std::move(got_type), stopped_at };
}

std::optional<type_representation> type_parser::generic_type()
{
  const auto name_token = eat_generic_type_token();
  if (!name_token) {
    return {};
  }

  const auto less_token = eat(token_type_t::less);
  if (!less_token) {
    return {};
  }

  auto value_type = parse_type();
  if (!value_type) {
    return {};
  }

  if (!expect_not_at_end()) {
    return {};
  }

  const auto greater_token = eat(token_type_t::greater);
  if (!greater_token) {
    return {};
  }

  token_container_t tokens;
  tokens.emplace_back(*name_token);
  tokens.emplace_back(*less_token);
  const auto& value_type_tokens = value_type->tokens();
  tokens.insert(std::end(tokens), std::cbegin(value_type_tokens),
                std::cend(value_type_tokens));
  tokens.emplace_back(*greater_token);

  type_representation::generic_type_name name{ std::move(tokens),
                                               { std::move(*value_type) } };

  const auto is_reference = (try_eat(token_type_t::amp) != std::nullopt);
  if (is_reference) {

    return type_representation{ std::move(name),
                                type_representation::is_reference_tag{} };
  }

  return type_representation{ std::move(name) };
}

bool type_parser::generic_type_starts() const
{
  return current_is_generic_type() && peek() == token_type_t::less;
}

bool type_parser::current_is_generic_type() const
{
  const auto generic_types = { token_type_t::kw_list,
                               token_type_t::kw_extern };

  return cmsl::contains(generic_types, curr_type());
}

std::optional<type_parser::token_t> type_parser::eat_generic_type_token()
{
  if (!current_is_generic_type()) {
    m_err_observer.raise_expected_type(get_token_for_error_report());
    return {};
  }

  return eat();
}

std::optional<type_representation> type_parser::simple_type()
{
  const auto qualified_name = eat_simple_type_name();
  if (!qualified_name) {
    return {};
  }

  const auto is_reference = (try_eat(token_type_t::amp) != std::nullopt);
  if (is_reference) {
    return type_representation{ *qualified_name,
                                type_representation::is_reference_tag{} };
  }

  return type_representation{ *qualified_name };
}

std::optional<ast::qualified_name> type_parser::eat_simple_type_name()
{
  const auto token_type = curr_type();

  if (is_builtin_simple_type(token_type) || possibly_qualified_name_starts()) {
    auto name = parse_possibly_qualified_name();
    if (name) {
      return qualified_name{ std::move(*name) };
    }
  }

  m_err_observer.raise_expected_type(get_token_for_error_report());
  return {};
}

bool type_parser::is_builtin_simple_type(token_type_t token_type) const
{
  return lexer::is_builtin_type(token_type);
}

std::optional<type_representation> type_parser::parse_type()
{
  return generic_type_starts() ? generic_type() : simple_type();
}
}
