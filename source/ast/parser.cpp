#include "ast/parser.hpp"

#include "ast/block_node.hpp"
#include "ast/break_node.hpp"
#include "ast/class_node.hpp"
#include "ast/conditional_node.hpp"
#include "ast/designated_initializers_node.hpp"
#include "ast/for_node.hpp"
#include "ast/if_else_node.hpp"
#include "ast/infix_nodes.hpp"
#include "ast/namespace_node.hpp"
#include "ast/return_node.hpp"
#include "ast/ternary_operator_node.hpp"
#include "ast/translation_unit_node.hpp"
#include "ast/type_parser.hpp"
#include "ast/type_parsing_result.hpp"
#include "ast/user_function_node.hpp"
#include "ast/variable_declaration_node.hpp"
#include "ast/while_node.hpp"
#include "common/algorithm.hpp"
#include "common/assert.hpp"
#include "errors/error.hpp"
#include "errors/errors_observer.hpp"

#include <map>

namespace cmsl::ast {
parser::parser(errors::errors_observer& err_observer, cmsl::source_view source,
               const token_container_t& tokens)
  : parser_utils{ m_errors_reporter, tokens.cbegin(), tokens.cend() }
  , m_errors_reporter{ err_observer, source }
{
}

std::unique_ptr<ast_node> parser::parse_translation_unit()
{
  const auto stop_condition = [this] { return is_at_end(); };
  auto nodes = parse_top_level_nodes(stop_condition);
  if (!nodes) {
    return nullptr;
  }

  return std::make_unique<translation_unit_node>(std::move(*nodes));
}

std::unique_ptr<ast_node> parser::constructor(token_t class_name)
{
  auto type_name = eat(token_type_t::identifier);
  if (!type_name) {
    return nullptr;
  }

  auto param_values = param_declarations();
  if (!param_values) {
    return nullptr;
  }

  auto block_expr = parse_block();
  if (!block_expr) {
    return nullptr;
  }

  // Let's pretend that ctors return something.
  return std::make_unique<user_function_node>(
    type_representation{ class_name }, *type_name, param_values->open_paren,
    std::move(param_values->params), param_values->close_paren,
    std::move(block_expr));
}

std::unique_ptr<ast_node> parser::parse_class()
{
  const auto class_kw = eat(token_type_t::kw_class);
  if (!class_kw) {
    return nullptr;
  }

  const auto name = eat(token_type_t::identifier);
  if (!name) {
    return nullptr;
  }

  const auto open_brace = eat(token_type_t::open_brace);
  if (!open_brace) {
    return nullptr;
  }

  std::vector<std::unique_ptr<ast_node>> class_nodes;

  while (!current_is(token_type_t::close_brace)) {
    const auto class_ctor_starts = current().str() == name->str();
    if (class_ctor_starts) {
      auto ctor = constructor(*name);
      if (!ctor) {
        return nullptr;
      }
      class_nodes.emplace_back(std::move(ctor));
    }
    if (function_declaration_starts()) {
      auto fun = parse_function();
      if (!fun) {
        return nullptr;
      }

      class_nodes.emplace_back(std::move(fun));
    } else {
      auto member = parse_standalone_variable_declaration();
      if (!member) {
        return nullptr;
      }

      class_nodes.emplace_back(std::move(member));
    }
  }

  const auto close_brace = eat(token_type_t::close_brace);
  if (!close_brace) {
    return nullptr;
  }

  const auto semicolon = eat(token_type_t::semicolon);
  if (!semicolon) {
    return nullptr;
  }

  return std::make_unique<class_node>(*class_kw, *name, *open_brace,
                                      std::move(class_nodes), *close_brace,
                                      *semicolon);
}

std::unique_ptr<ast_node> parser::parse_function()
{
  const auto ty = parse_type();
  if (!ty) {
    return nullptr;
  }

  const auto name = eat(token_type_t::identifier);
  if (!name) {
    return nullptr;
  }

  auto param_values = param_declarations();
  if (!param_values) {
    return nullptr;
  }

  auto block_node = parse_block();

  if (!block_node) {
    return nullptr;
  }

  return std::make_unique<user_function_node>(
    *ty, *name, param_values->open_paren, std::move(param_values->params),
    param_values->close_paren, std::move(block_node));
}

std::unique_ptr<ast_node> parser::parse_return_node()
{
  const auto return_kw = eat(token_type_t::kw_return);
  if (!return_kw) {
    return nullptr;
  }

  auto e = parse_expr();
  if (!e) {
    return nullptr;
  }

  const auto semicolon = eat(token_type_t::semicolon);
  if (!semicolon) {
    return nullptr;
  }

  return std::make_unique<return_node>(*return_kw, std::move(e), *semicolon);
}

bool parser::declaration_starts() const
{
  parse_errors_sink errs_sink;
  type_parser p{ errs_sink, current_iterator(), end_iterator() };
  const auto parsing_result = p.type();

  if (!parsing_result.ty) {
    return false;
  }

  return type_of_token_is(parsing_result.stopped_at, token_type_t::identifier);
}

std::unique_ptr<conditional_node> parser::get_conditional_node()
{
  const auto open_paren = eat(token_type_t::open_paren);
  if (!open_paren) {
    return nullptr;
  }

  auto condition = parse_expr();
  if (!condition) {
    return nullptr;
  }

  const auto close_paren = eat(token_type_t::close_paren);
  if (!close_paren) {
    return nullptr;
  }

  auto b = parse_block();
  if (!b) {
    return nullptr;
  }

  return std::make_unique<conditional_node>(*open_paren, std::move(condition),
                                            *close_paren, std::move(b));
}

std::unique_ptr<ast_node> parser::parse_if_else_node()
{
  if (!current_is(token_type_t::kw_if)) {
    // Expected if
    m_errors_reporter.raise_expected_keyword(current(), token_type_t::kw_if);
    return nullptr;
  }

  std::vector<if_else_node::if_values> ifs;

  std::optional<token_t> else_kw;
  while (const auto if_kw = try_eat(token_type_t::kw_if)) {
    auto if_node = get_conditional_node();
    if (!if_node) {
      return nullptr;
    }

    auto if_values =
      if_else_node::if_values{ else_kw, *if_kw, std::move(if_node) };
    ifs.emplace_back(std::move(if_values));

    if (current_is(token_type_t::kw_else)) {
      const auto next_type = peek();
      if (next_type == token_type_t::kw_if) {
        else_kw = eat(token_type_t::kw_else);
      }
    }
  }

  std::optional<if_else_node::last_else_value> last_else;
  if (const auto last_else_kw = try_eat(token_type_t::kw_else)) {
    auto else_block = parse_block();
    if (!else_block) {
      return nullptr;
    }

    last_else =
      if_else_node::last_else_value{ *last_else_kw, std::move(else_block) };
  }

  return std::make_unique<if_else_node>(std::move(ifs), std::move(last_else));
}

std::unique_ptr<ast_node> parser::parse_while_node()
{
  const auto while_kw = eat(token_type_t::kw_while);
  if (!while_kw) {
    return nullptr;
  }

  auto conditional_node = get_conditional_node();
  if (!conditional_node) {
    return nullptr;
  }

  return std::make_unique<while_node>(*while_kw, std::move(conditional_node));
}

std::unique_ptr<block_node> parser::parse_block()
{
  const auto open_brace = eat(token_type_t::open_brace);
  if (!open_brace) {
    return nullptr;
  }

  std::vector<std::unique_ptr<ast_node>> nodes;

  while (!is_at_end() && !current_is(token_type_t::close_brace)) {
    std::unique_ptr<ast_node> node;

    if (current_is(token_type_t::kw_return)) {
      node = parse_return_node();
    } else if (declaration_starts()) {
      node = parse_standalone_variable_declaration();
    } else if (current_is(token_type_t::kw_if)) {
      node = parse_if_else_node();
    } else if (current_is(token_type_t::kw_while)) {
      node = parse_while_node();
    } else if (current_is(token_type_t::kw_for)) {
      node = parse_for_node();
    } else if (current_is(token_type_t::kw_break)) {
      node = parse_break();
    } else if (current_is(token_type_t::open_brace)) {
      node = parse_block();
    } else {
      node = parse_expr();
      if (!eat(token_type_t::semicolon)) {
        return nullptr;
      }
    }

    if (!node) {
      return nullptr;
    }

    nodes.emplace_back(std::move(node));
  }

  const auto close_brace = eat(token_type_t::close_brace);
  if (!close_brace) {
    return nullptr;
  }

  return std::make_unique<block_node>(*open_brace, std::move(nodes),
                                      *close_brace);
}

std::optional<param_declaration> parser::get_param_declaration()
{
  auto t = parse_type();

  if (!t) {
    return {};
  }

  auto name = eat(token_type_t::identifier);
  if (!name) {
    return {};
  }

  return param_declaration{ *t, *name };
}

bool parser::prepare_for_next_parameter_declaration()
{
  if (!expect_not_at_end()) {
    return false;
  }

  if (!current_is(token_type_t::comma)) {
    return true;
  }

  // At this point we have comma, so we expect next parameter

  if (!eat(token_type_t::comma)) {
    return false;
  }

  if (!expect_not_at_end()) {
    return false;
  }

  if (!is_at_end() && current_is(token_type_t::close_paren)) {
    // Missed last parameter declaration
    m_errors_reporter.raise_expected_parameter_declaration(current());
    return false;
  }

  return true;
}

std::optional<parser::param_list_values> parser::param_declarations()
{
  std::vector<param_declaration> params;

  const auto open_paren = eat(token_type_t::open_paren);
  if (!open_paren) {
    return {};
  }

  while (true) {
    if (is_at_end()) {
      m_errors_reporter.raise_unexpected_end_of_file(prev());
      return {};
    }

    if (current_is(token_type_t::close_paren)) {
      // End of parameters
      break;
    }

    const auto param_decl = get_param_declaration();
    if (!param_decl) {
      return {};
    }

    params.push_back(std::move(*param_decl));

    if (!prepare_for_next_parameter_declaration()) {
      return {};
    }
  }

  const auto close_paren = eat(token_type_t::close_paren);
  if (!close_paren) {
    return {};
  }

  return param_list_values{ *open_paren, std::move(params), *close_paren };
}

std::unique_ptr<variable_declaration_node> parser::parse_variable_declaration()
{
  const auto ty = parse_type();
  if (!ty) {
    return nullptr;
  }

  const auto name = eat(token_type_t::identifier);
  if (!name) {
    return nullptr;
  }

  std::optional<standalone_variable_declaration_node::initialization_values_t>
    initialization_vals;
  if (const auto equal = try_eat(token_type_t::equal)) {
    auto initialization = parse_expr();
    if (!initialization) {
      return nullptr;
    }

    initialization_vals =
      standalone_variable_declaration_node::initialization_values_t{
        *equal, std::move(initialization)
      };
  }

  return std::make_unique<variable_declaration_node>(
    *ty, *name, std::move(initialization_vals));
}

std::unique_ptr<ast_node> parser::parse_standalone_variable_declaration()
{
  auto variable_decl = parse_variable_declaration();
  if (!variable_decl) {
    return nullptr;
  }

  const auto semicolon = eat(token_type_t::semicolon);
  if (!semicolon) {
    return nullptr;
  }

  return std::make_unique<standalone_variable_declaration_node>(
    std::move(variable_decl), *semicolon);
}

std::optional<type_representation> parser::parse_type()
{
  type_parser ty_parser{ m_errors_reporter, current_iterator(),
                         end_iterator() };
  auto parsing_result = ty_parser.type();
  adjust_current_iterator(parsing_result.stopped_at);
  return std::move(parsing_result.ty);
}

std::optional<parser::token_t> parser::eat_function_call_name()
{
  if (!current_is_name_of_function_call()) {
    m_errors_reporter.raise_unexpected_token(current());
    return {};
  }

  const auto fun_name_token = eat();
  if (!fun_name_token) {
    return {};
  }

  return *fun_name_token;
}

std::unique_ptr<ast_node> parser::parse_operator(unsigned precedence)
{
  static const std::map<unsigned, std::vector<token_type_t>> operators{
    { 2, { token_type_t::dot } },
    { 3, { token_type_t::exclaim } },
    { 5, { token_type_t::star, token_type_t::slash, token_type_t::percent } },
    { 6, { token_type_t::plus, token_type_t::minus } },
    { 9,
      { token_type_t::less, token_type_t::lessequal, token_type_t::greater,
        token_type_t::greaterequal } },
    { 10, { token_type_t::equalequal, token_type_t::exclaimequal } },
    { 11, { token_type_t::amp } },
    { 12, { token_type_t::xor_ } },
    { 13, { token_type_t::pipe } },
    { 14, { token_type_t::ampamp } },
    { 15, { token_type_t::pipepipe } },
    { 16,
      { token_type_t::equal, token_type_t::plusequal, token_type_t::minusequal,
        token_type_t::starequal, token_type_t::slashequal,
        token_type_t::ampequal, token_type_t::xorequal,
        token_type_t::pipeequal } }
  };

  const auto is_current_same_precedence_operator =
    [current_precedence = precedence, this] {
      const auto found = operators.find(current_precedence);
      if (found == std::cend(operators)) {
        return false;
      }

      const auto& ops = found->second;

      return contains(ops, curr_type());
    };

  const auto get_next_precedence = [current_precedence = precedence] {
    const auto found = operators.find(current_precedence);
    const auto higher_precedence_it = std::prev(found);
    return higher_precedence_it->first;
  };

  if (precedence == k_min_precedence) {
    auto f = parse_factor();
    if (!f) {
      return nullptr;
    }

    while (!is_at_end() && is_current_same_precedence_operator()) {
      const auto dot = eat(); // dot
      auto lhs = std::move(f);
      if (current_is_class_member_access()) {
        const auto member_name = eat();
        f = std::make_unique<class_member_access_node>(std::move(lhs), *dot,
                                                       *member_name);
      } else // class member function call
      {
        auto vals = get_function_call_values();
        if (!vals) {
          return nullptr;
        }

        f = std::make_unique<member_function_call_node>(
          std::move(lhs), *dot, vals->name, vals->open_paren,
          std::move(vals->params), vals->close_paren);
      }
    }

    return f;
  } else {
    const auto next_precedence = get_next_precedence();
    auto f = parse_operator(next_precedence);
    if (!f) {
      return nullptr;
    }

    while (!is_at_end() && is_current_same_precedence_operator()) {
      const auto op = current();
      eat(); // eat operator

      auto rhs = parse_operator(next_precedence);
      if (!rhs) {
        return nullptr;
      }

      auto lhs = std::move(f);
      f = std::make_unique<binary_operator_node>(std::move(lhs), op,
                                                 std::move(rhs));
    }

    return f;
  }
}

bool parser::current_is_class_member_access() const
{
  // At this point we know that we're after the dot token. Test whether it's a
  // member function call or just member access.
  return !current_is_function_call();
}

bool parser::current_is_function_call() const
{
  return current_is_name_of_function_call() &&
    next_is(token_type_t::open_paren);
}

bool parser::current_is_fundamental_value() const
{
  switch (curr_type()) {
    case lexer::token_type::integer:
    case lexer::token_type::double_:
    case lexer::token_type::string:
    case lexer::token_type::kw_true:
    case lexer::token_type::kw_false:
      return true;

    default:
      return false;
  }
}

bool parser::current_is_possibly_qualified_name() const
{
  return current_is(token_type_t::coloncolon) ||
    current_is(token_type_t::identifier);
}

std::unique_ptr<ast_node> parser::fundamental_value()
{
  const auto token = eat();

  if (!token) {
    return nullptr;
  }

  switch (token->get_type()) {
    case lexer::token_type::integer:
      return std::make_unique<int_value_node>(*token);

    case lexer::token_type::double_:
      return std::make_unique<double_value_node>(*token);

    case lexer::token_type::string:
      return std::make_unique<string_value_node>(*token);

    case lexer::token_type::kw_true:
    case lexer::token_type::kw_false:
      return std::make_unique<bool_value_node>(*token);

    default:
      return nullptr;
  }
}

std::unique_ptr<ast_node> parser::parse_factor()
{
  if (current_is_function_call()) {
    return function_call();
  } else if (current_is_fundamental_value()) {
    return fundamental_value();
  } else if (try_eat(token_type_t::open_paren)) {
    auto e = parse_expr();
    eat(token_type_t::close_paren);
    return e;
  } else if (current_is_possibly_qualified_name()) {
    return parse_possibly_qualified_name();
  } else if (current_is(token_type_t::open_brace)) {
    return parse_expression_starting_from_brace();
  }

  m_errors_reporter.raise_unexpected_token(get_token_for_error_report());
  return nullptr;
}

std::unique_ptr<ast_node> parser::parse_expr()
{
  auto operator_expr = parse_operator();

  if (const auto dot = try_eat(token_type_t::dot)) {
    if (current_is_class_member_access()) {
      const auto member_name = eat();
      return std::make_unique<class_member_access_node>(
        std::move(operator_expr), *dot, *member_name);
    } else // class member function call
    {
      auto vals = get_function_call_values();

      if (!vals) {
        return nullptr;
      }

      return std::make_unique<member_function_call_node>(
        std::move(operator_expr), *dot, vals->name, vals->open_paren,
        std::move(vals->params), vals->close_paren);
    }
  } else if (const auto question = try_eat(token_type_t::question)) {
    return parse_rest_of_ternary_operator(std::move(operator_expr), *question);
  }

  return operator_expr;
}

std::optional<parser::function_call_values> parser::get_function_call_values()
{
  function_call_values vals;

  const auto name = *eat_function_call_name();
  auto params = parameter_list();
  if (!params) {
    return {};
  }

  std::move(*params);

  return function_call_values{ name, params->open_paren,
                               std::move(params->params),
                               params->close_paren };
}

std::optional<std::vector<std::unique_ptr<ast_node>>>
parser::comma_separated_expression_list(token_type_t valid_end_of_list_token)
{
  std::vector<std::unique_ptr<ast_node>> exprs;

  while (!is_at_end() && !current_is(valid_end_of_list_token)) {
    auto expression = parse_expr();
    if (!expression) {
      // Unexpected token e.g. semicolon
      return {};
    }

    exprs.emplace_back(std::move(expression));

    if (try_eat(token_type_t::comma)) {
      if (is_at_end() || current_is(valid_end_of_list_token)) {
        const auto tok = is_at_end() ? prev() : current();
        m_errors_reporter.raise_expected_expression(tok);
        return {};
      }
    }
  }

  return std::move(exprs);
}

std::optional<parser::call_param_list_values> parser::parameter_list()
{
  const auto open_paren = eat(token_type_t::open_paren);
  if (!open_paren) {
    return {};
  }

  auto params = comma_separated_expression_list(token_type_t::close_paren);
  if (!params) {
    return {};
  }

  const auto close_paren = eat(token_type_t::close_paren);
  if (!close_paren) {
    return {};
  }

  return call_param_list_values{ *open_paren, std::move(*params),
                                 *close_paren };
}

std::unique_ptr<ast_node> parser::function_call()
{
  auto vals = get_function_call_values();
  if (!vals) {
    return nullptr;
  }

  return std::make_unique<function_call_node>(
    vals->name, vals->open_paren, std::move(vals->params), vals->close_paren);
}

bool parser::function_declaration_starts() const
{
  parse_errors_sink errs_sink;
  type_parser p{ errs_sink, current_iterator(), end_iterator() };
  const auto parsing_result = p.type();

  if (!parsing_result.ty) {
    return false;
  }

  return type_of_token_is(parsing_result.stopped_at,
                          token_type_t::identifier) &&
    next_to_is(parsing_result.stopped_at, token_type_t::open_paren);
}

std::unique_ptr<ast_node> parser::parse_initializer_list()
{
  auto open_brace = eat(token_type_t::open_brace);
  if (!open_brace) {
    return nullptr;
  }

  auto values = comma_separated_expression_list(token_type_t::close_brace);
  if (!values) {
    return {};
  }

  auto close_brace = eat(token_type_t::close_brace);
  if (!close_brace) {
    return nullptr;
  }

  return std::make_unique<initializer_list_node>(
    *open_brace, std::move(*values), *close_brace);
}

std::unique_ptr<ast_node> parser::parse_for_node()
{

  const auto for_kw = eat(token_type_t::kw_for);
  if (!for_kw) {
    return nullptr;
  }

  const auto open_paren = eat(token_type_t::open_paren);
  if (!open_paren) {
    return nullptr;
  }

  auto init = parse_for_init();
  if (!init.has_value()) {
    return nullptr;
  }

  const auto init_semicolon = eat(token_type_t::semicolon);
  if (!init_semicolon) {
    return nullptr;
  }

  auto condition = parse_for_condition();
  if (!condition.has_value()) {
    return nullptr;
  }

  const auto condition_semicolon = eat(token_type_t::semicolon);
  if (!condition_semicolon) {
    return nullptr;
  }

  auto iteration = parse_for_iteration();
  if (!iteration.has_value()) {
    return nullptr;
  }

  const auto close_paren = eat(token_type_t::close_paren);
  if (!close_paren) {
    return nullptr;
  }

  auto body = parse_block();
  if (!body) {
    return nullptr;
  }

  return std::make_unique<for_node>(
    *for_kw, *open_paren, std::move(*init), *init_semicolon,
    std::move(*condition), *condition_semicolon, std::move(*iteration),
    *close_paren, std::move(body));
}

std::optional<std::unique_ptr<ast_node>> parser::parse_for_init()
{
  std::unique_ptr<ast_node> node;

  if (declaration_starts()) {
    node = parse_variable_declaration();
  } else if (!current_is(token_type_t::semicolon)) {
    node = parse_expr();
  } else {
    return nullptr;
  }

  if (!node) {
    return std::nullopt;
  }

  return std::move(node);
}

std::optional<std::unique_ptr<ast_node>> parser::parse_for_condition()
{
  if (current_is(token_type_t::semicolon)) {
    return nullptr;
  }

  auto node = parse_expr();
  if (!node) {
    return std::nullopt;
  }

  return std::move(node);
}

std::optional<std::unique_ptr<ast_node>> parser::parse_for_iteration()
{
  if (current_is(token_type_t::close_paren)) {
    return nullptr;
  }

  auto node = parse_expr();
  if (!node) {
    return std::nullopt;
  }

  return std::move(node);
}

std::unique_ptr<break_node> parser::parse_break()
{
  const auto break_kw = eat(token_type_t::kw_break);
  if (!break_kw) {
    return nullptr;
  }

  const auto semicolon = eat(token_type_t::semicolon);
  if (!semicolon) {
    return nullptr;
  }

  return std::make_unique<break_node>(*break_kw, *semicolon);
}

std::unique_ptr<namespace_node> parser::parse_namespace()
{
  const auto namespace_kw = eat(token_type_t::kw_namespace);
  if (!namespace_kw) {
    return nullptr;
  }

  auto names = parse_namespace_declaration_names();
  if (names.empty()) {
    return nullptr;
  }

  const auto open_bracket = eat(token_type_t::open_brace);
  if (!open_bracket) {
    return nullptr;
  }

  const auto stop_condition = [this] {
    return curr_type() == token_type_t::close_brace;
  };
  auto nodes = parse_top_level_nodes(stop_condition);
  if (!nodes) {
    return nullptr;
  }

  const auto close_brace = eat(token_type_t::close_brace);
  if (!close_brace) {
    return nullptr;
  }

  return std::make_unique<namespace_node>(*namespace_kw, std::move(names),
                                          *open_bracket, std::move(*nodes),
                                          *close_brace);
}

template <typename StopCondition>
std::optional<std::vector<std::unique_ptr<ast_node>>>
parser::parse_top_level_nodes(StopCondition&& stop_condition)
{
  std::vector<std::unique_ptr<ast_node>> nodes;

  while (!stop_condition()) {
    std::unique_ptr<ast_node> node;

    if (function_declaration_starts()) {
      node = parse_function();
    } else if (current_is(token_type_t::kw_class)) {
      node = parse_class();
    } else if (current_is(token_type_t::kw_namespace)) {
      node = parse_namespace();
    } else {
      node = parse_standalone_variable_declaration();
    }

    if (!node) {
      return std::nullopt;
    }

    nodes.emplace_back(std::move(node));
  }

  return std::move(nodes);
}

std::vector<name_with_coloncolon> parser::parse_namespace_declaration_names()
{
  std::vector<name_with_coloncolon> names;

  while (true) {
    auto name = eat(token_type_t::identifier);
    if (!name) {
      return {};
    }

    const auto coloncolon = try_eat(token_type_t::coloncolon);
    if (!coloncolon) {
      names.emplace_back(name_with_coloncolon{ *name });
      break;
    }

    names.emplace_back(name_with_coloncolon{ *name, *coloncolon });
  }

  return names;
}

std::unique_ptr<id_node> parser::parse_possibly_qualified_name()
{
  id_node::names_t names;

  const auto starts_with_global_scope_access =
    current_is(token_type_t::coloncolon);
  if (starts_with_global_scope_access) {
    auto name = lexer::make_token(token_type_t::identifier, "");
    auto coloncolon = eat(token_type_t::coloncolon);

    names.emplace_back(name_with_coloncolon{ name, coloncolon });
  }

  while (true) {
    auto name = eat(token_type_t::identifier);
    if (!name) {
      return nullptr;
    }

    if (auto coloncolon = try_eat(token_type_t::coloncolon)) {
      names.emplace_back(name_with_coloncolon{ *name, coloncolon });
    } else {
      names.emplace_back(name_with_coloncolon{ *name });
      break;
    }
  }

  return std::make_unique<id_node>(std::move(names));
}

std::unique_ptr<ternary_operator_node> parser::parse_rest_of_ternary_operator(
  std::unique_ptr<ast_node> condition, token_t question)
{
  auto true_ = parse_expr();
  if (!true_) {
    return nullptr;
  }

  const auto colon = eat(token_type_t::colon);
  if (!colon) {
    return nullptr;
  }

  auto false_ = parse_expr();
  if (!false_) {
    return nullptr;
  }

  return std::make_unique<ternary_operator_node>(std::move(condition),
                                                 question, std::move(true_),
                                                 *colon, std::move(false_));
}
std::unique_ptr<ast_node> parser::parse_expression_starting_from_brace()
{
  if (next_is(token_type_t::dot)) {
    return parse_designated_initializers();
  } else {
    return parse_initializer_list();
  }
}

std::unique_ptr<designated_initializers_node>
parser::parse_designated_initializers()
{
  const auto open_brace = eat(token_type_t::open_brace);
  if (!open_brace) {
    return nullptr;
  }

  using initializer_t = designated_initializers_node::initializer;
  std::vector<initializer_t> initializers;

  while (const auto dot = try_eat(token_type_t::dot)) {
    const auto name = eat(token_type_t::identifier);
    if (!name) {
      return nullptr;
    }

    const auto equal = eat(token_type_t::equal);
    if (!equal) {
      return nullptr;
    }

    auto initialization = parse_expr();
    if (!initialization) {
      return nullptr;
    }

    std::optional<token_t> comma;

    if (const auto tried_comma = try_eat(token_type_t::comma)) {
      comma = tried_comma;

      if (!current_is(token_type_t::dot)) {
        m_errors_reporter.raise_expected_designated_initializer(
          get_token_for_error_report());
        return nullptr;
      }

    } else if (current_is(token_type_t::dot)) {
      m_errors_reporter.raise_expected_token(get_token_for_error_report(),
                                             token_type_t::comma);
      return nullptr;
    }

    auto initializer =
      initializer_t{ .dot = *dot,
                     .name = *name,
                     .equal = *equal,
                     .initialization = std::move(initialization),
                     .comma = comma };
    initializers.emplace_back(std::move(initializer));
  }

  const auto close_brace = eat(token_type_t::close_brace);
  if (!close_brace) {
    return nullptr;
  }

  return std::make_unique<designated_initializers_node>(
    *open_brace, std::move(initializers), *close_brace);
}
}
