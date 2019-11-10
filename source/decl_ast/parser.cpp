#include "decl_ast/parser.hpp"

#include "decl_ast/ast_nodes.hpp"

namespace cmsl::decl_ast {

parser::parser(errors::errors_observer& err_observer,
               strings_container& strings_container, cmsl::source_view source,
               const lexer::token_container_t& tokens)
  : parser_utils{ m_errors_reporter, tokens.cbegin(), tokens.cend() }
  , m_errors_reporter{ err_observer, source }
{
}

std::unique_ptr<component_node> parser::parse_component()
{
  const auto name = eat(token_type_t::identifier);
  if (!name) {
    return nullptr;
  }

  const auto open_brace = eat(token_type_t::open_brace);
  if (!open_brace) {
    return nullptr;
  }

  auto nodes = parse_component_nodes();
  if (!nodes) {
    return nullptr;
  }

  const auto close_brace = eat(token_type_t::close_brace);
  if (!close_brace) {
    return nullptr;
  }

  return std::make_unique<component_node>(*name, *open_brace,
                                          std::move(*nodes), *close_brace);
}

std::optional<std::vector<std::unique_ptr<ast_node>>>
parser::parse_component_nodes()
{
  std::vector<std::unique_ptr<ast_node>> nodes;

  while (!current_is(token_type_t::close_brace)) {
    std::unique_ptr<ast_node> node;

    if (component_declaration_starts()) {
      node = parse_component();
    } else if (property_declaration_starts()) {
      node = parse_property();
    } else {
      if (!expect_not_at_end()) {
        return std::nullopt;
      }

      m_errors_reporter.raise_expected_component_or_property(
        get_token_for_error_report());
    }

    if (node == nullptr) {
      return std::nullopt;
    }

    nodes.emplace_back(std::move(node));
  }

  return std::move(nodes);
}

bool parser::component_declaration_starts() const
{
  return current_is(token_type_t::identifier) &&
    next_is(token_type_t::open_brace);
}

bool parser::property_declaration_starts() const
{
  return current_is(token_type_t::identifier) && next_is(token_type_t::colon);
}

std::unique_ptr<property_node> parser::parse_property()
{
  const auto name = eat(token_type_t::identifier);
  if (!name) {
    return nullptr;
  }

  const auto assignment = eat(token_type_t::colon);
  if (!assignment) {
    return nullptr;
  }

  auto value = parse_value();
  if (!value) {
    return nullptr;
  }

  return std::make_unique<property_node>(*name, *assignment, std::move(value));
}

std::unique_ptr<ast_node> parser::parse_value()
{
  if (current_is(token_type_t::kw_true) ||
      current_is(token_type_t::kw_false)) {
    const auto token = *eat();
    return std::make_unique<bool_value_node>(token);
  }
  if (current_is(token_type_t::integer)) {
    const auto token = *eat();
    return std::make_unique<int_value_node>(token);
  }
  if (current_is(token_type_t::double_)) {
    const auto token = *eat();
    return std::make_unique<double_value_node>(token);
  }
  if (current_is(token_type_t::string)) {
    const auto token = *eat();
    return std::make_unique<string_value_node>(token);
  }
  if (current_is(token_type_t::open_square)) {
    return parse_list();
  }

  return nullptr;
}

std::unique_ptr<list_node> parser::parse_list()
{
  const auto open_square = eat(token_type_t::open_square);
  if (!open_square) {
    return nullptr;
  }
  list_node::nodes_t values;

  while (!current_is(token_type_t::close_square)) {
    if (current_is(token_type_t::open_square)) {
      m_errors_reporter.raise_unexpected_list_in_list(
        get_token_for_error_report());
      return nullptr;
    }

    auto value = parse_value();
    if (!value) {
      return nullptr;
    }

    values.emplace_back(std::move(value));

    const auto next_is_value_not_separated_by_comma =
      !try_eat(token_type_t::comma) && !current_is(token_type_t::close_square);
    if (next_is_value_not_separated_by_comma) {
      m_errors_reporter.raise_expected_token(get_token_for_error_report(),
                                             token_type_t::comma);
      return nullptr;
    }

    if (!expect_not_at_end()) {
      return nullptr;
    }
  }

  const auto close_square = eat(token_type_t::close_square);
  if (!close_square) {
    return nullptr;
  }

  return std::make_unique<list_node>(*open_square, std::move(values),
                                     *close_square);
}
}
