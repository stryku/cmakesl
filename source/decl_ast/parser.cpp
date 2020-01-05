#include "decl_ast/parser.hpp"
#include "common/algorithm.hpp"
#include "common/assert.hpp"
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
  const auto name = eat();
  if (name->get_type() != token_type_t::identifier &&
      name->get_type() != token_type_t::kw_executable) {
    m_errors_reporter.raise_expected_token(get_token_for_error_report(),
                                           token_type_t::identifier);
    return nullptr;
  }

  std::optional<token_t> derived_type_name;
  if (const auto colon = try_eat(token_type_t::colon)) {
    derived_type_name = eat(token_type_t::identifier);
    if (!derived_type_name) {
      // Todo: raise error
      return nullptr;
    }
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

  return std::make_unique<component_node>(
    *name, derived_type_name, *open_brace, std::move(*nodes), *close_brace);
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
  // Todo: check for `:` and nested properties
  return current_is(token_type_t::identifier);
}

std::unique_ptr<property_node> parser::parse_property()
{
  auto property_access = parse_property_access();
  if (!property_access) {
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

  return std::make_unique<property_node>(std::move(property_access),
                                         *assignment, std::move(value));
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
  if (current().str() == "cmake_variables") {
    return parse_cmake_variable_access();
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

std::unique_ptr<property_access_node> parser::parse_property_access()
{
  std::vector<parser::token_t> property_names;

  while (const auto name = eat(token_type_t::identifier)) {
    property_names.emplace_back(*name);

    if (!current_is(token_type_t::dot)) {
      return std::make_unique<property_access_node>(std::move(property_names));
    }

    (void)eat(token_type_t::dot); // dot
  }

  // Todo: raise expected identifier.
  return nullptr;
}

std::unique_ptr<cmake_variable_access_node>
parser::parse_cmake_variable_access()
{
  const auto cmake_variables = eat();
  if (!cmake_variables) {
    return nullptr;
  }

  const auto dot = eat(token_type_t::dot);
  if (!dot) {
    return nullptr;
  }

  const auto variable_name = eat(token_type_t::identifier);
  if (!variable_name) {
    return nullptr;
  }

  const auto dot2 = eat(token_type_t::dot);
  if (!dot2) {
    return nullptr;
  }

  const auto as_type = eat(token_type_t::identifier);
  if (!as_type) {
    return nullptr;
  }

  const auto expected = { "as_bool", "as_int", "as_double", "as_string",
                          "as_list" };
  if (!contains(expected, as_type->str())) {
    m_errors_reporter.raise_expected_as_type(*as_type);
    return nullptr;
  }

  return std::make_unique<cmake_variable_access_node>(
    *cmake_variables, *variable_name, *as_type);
}
}
