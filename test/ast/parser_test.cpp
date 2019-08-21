#include "ast/ast_node_visitor.hpp"
#include "ast/block_node.hpp"
#include "ast/break_node.hpp"
#include "ast/class_node.hpp"
#include "ast/conditional_node.hpp"
#include "ast/designated_initializers_node.hpp"
#include "ast/for_node.hpp"
#include "ast/if_else_node.hpp"
#include "ast/infix_nodes.hpp"
#include "ast/namespace_node.hpp"
#include "ast/parser.hpp"
#include "ast/return_node.hpp"
#include "ast/ternary_operator_node.hpp"
#include "ast/translation_unit_node.hpp"
#include "ast/user_function_node.hpp"
#include "ast/variable_declaration_node.hpp"
#include "ast/while_node.hpp"
#include "errors/errors_observer.hpp"

#include "test/common/tokens.hpp"

#include <gmock/gmock.h>

namespace cmsl::ast::test {
using token_t = cmsl::lexer::token;
using token_type_t = cmsl::lexer::token_type;
using tokens_container_t = cmsl::lexer::token_container_t;
using parser_t = cmsl::ast::parser;

using namespace cmsl::test::common;
errors::errors_observer dummy_err_observer;

const auto tmp_token = token_identifier("");

using ::testing::NotNull;
using ::testing::Ref;
using ::testing::Eq;

class ast_tree_representation_visitor : public ast_node_visitor
{
public:
  void visit(const block_node& node) override
  {
    m_result += "block{";
    for (const auto& n : node.nodes()) {
      n->visit(*this);
    }
    m_result += "}";
  }

  void visit(const class_node& node) override
  {
    m_result += "class{name:" + std::string{ node.name().str() } + ";members:";

    for (const auto& n : node.nodes()) {
      n->visit(*this);
    }

    m_result += "}";
  }

  void visit(const conditional_node& node) override
  {
    m_result += "conditional{condition:";
    node.condition().visit(*this);
    m_result += ";block:";
    node.body().visit(*this);
    m_result += "}";
  }

  void visit(const if_else_node& node) override
  {
    m_result += "if_else{ifs:";
    for (const auto& if_ : node.ifs()) {
      if_.conditional->visit(*this);
    }
    m_result += ";else:";
    auto else_node = node.else_body();
    if (else_node) {
      else_node->visit(*this);
    }
    m_result += "}";
  }

  void visit(const binary_operator_node& node) override
  {
    m_result += "binary_operator{lhs:";
    node.lhs().visit(*this);
    m_result += ";operator:" + std::string{ node.operator_().str() } + ";rhs:";
    node.rhs().visit(*this);
    m_result += "}";
  }

  void visit(const class_member_access_node& node) override
  {
    m_result += "class_member_access{lhs:";
    node.lhs().visit(*this);
    m_result +=
      ";member_name:" + std::string{ node.member_name().str() } + "}";
  }

  void visit(const function_call_node& node) override
  {
    m_result +=
      "function_call{name:" + std::string{ node.name().str() } + ";params:";
    for (const auto& param : node.param_nodes()) {
      param->visit(*this);
    }
    m_result += "}";
  }

  void visit(const member_function_call_node& node) override
  {
    m_result +=
      "function_call{name:" + std::string{ node.name().str() } + ";params:";
    m_result += "function_call{lhs:";
    node.lhs().visit(*this);
    m_result += ";name:" + std::string{ node.name().str() } + ";params:";
    for (const auto& param : node.param_nodes()) {
      param->visit(*this);
    }
    m_result += "}";
  }

  void visit(const bool_value_node& node) override
  {
    m_result += "bool_value{" + std::string{ node.token().str() } + "}";
  }

  void visit(const int_value_node& node) override
  {
    m_result += "int_value{" + std::string{ node.token().str() } + "}";
  }

  void visit(const double_value_node& node) override
  {
    m_result += "double_value{" + std::string{ node.token().str() } + "}";
  }

  void visit(const string_value_node& node) override
  {
    m_result += "string_value{" + std::string{ node.token().str() } + "}";
  }

  void visit(const id_node& node) override
  {
    m_result += "id{";

    for (const auto& name : node.names()) {
      m_result += name.name.str();
      if (name.coloncolon) {
        m_result += "::";
      }
    }
    m_result += "}";
  }

  void visit(const return_node& node) override
  {
    m_result += "return{";
    node.expression().visit(*this);
    m_result += "}";
  }

  void visit(const translation_unit_node& node) override
  {
    m_result += "translation_unit{";
    for (const auto& n : node.nodes()) {
      n->visit(*this);
    }
    m_result += "}";
  }

  void visit(const namespace_node& node) override
  {
    m_result += "namespace_node{name:";
    for (const auto& name : node.names()) {
      m_result += name.name.str();
      if (name.coloncolon) {
        m_result += "::";
      }
    }

    m_result += ";nodes:";
    for (const auto& n : node.nodes()) {
      n->visit(*this);
    }
    m_result += "}";
  }

  void visit(const user_function_node& node) override
  {
    m_result += "user_function{return_type:";
    const auto ret_type_reference = node.return_type_representation();
    m_result += ret_type_reference.to_string() + ";name:";
    m_result += std::string{ node.name().str() } + ";params:";
    for (const auto& param_declaration : node.param_declarations()) {
      m_result += "param_declaration{type:" + param_declaration.ty.to_string();
      m_result += ";name:" + std::string{ param_declaration.name.str() } + "}";
    }
    m_result += ";body:";
    node.body().visit(*this);
    m_result += "}";
  }

  void visit(const variable_declaration_node& node) override
  {
    m_result += "variable_declaration{type:";
    const auto ret_type_reference = node.type();
    m_result += ret_type_reference.to_string() + ";name:";
    m_result += std::string{ node.name().str() } + ";initialization:";
    if (const auto init = node.initialization()) {
      init->visit(*this);
    }
    m_result += "}";
  }

  void visit(const standalone_variable_declaration_node& node) override
  {
    m_result += "standalone_variable_declaration_node{";
    node.variable_declaration().visit(*this);
    m_result += "}";
  }

  void visit(const while_node& node) override
  {
    m_result += "while{";
    node.node().visit(*this);
    m_result += "}";
  }

  void visit(const initializer_list_node& node) override
  {
    m_result += "initializer_list_node{";
    std::string separator = "";
    for (const auto& value : node.values()) {
      m_result += separator;
      value->visit(*this);
      separator = ';';
    }
    m_result += "}";
  }

  void visit(const for_node& node) override
  {
    m_result += "for{init:";

    if (node.init() == nullptr) {
      m_result += "null";
    } else {
      node.init()->visit(*this);
    }

    m_result += ";condition:";

    if (node.condition() == nullptr) {
      m_result += "null";
    } else {
      node.condition()->visit(*this);
    }

    m_result += ";iteration:";

    if (node.iteration() == nullptr) {
      m_result += "null";
    } else {
      node.iteration()->visit(*this);
    }

    m_result += ";body:";
    node.body().visit(*this);

    m_result += "}";
  }

  void visit(const break_node& node) override { m_result += "break{}"; }

  void visit(const ternary_operator_node& node) override
  {
    m_result += "ternary operator{condition:";

    node.condition().visit(*this);

    m_result += ";true:";
    node.true_().visit(*this);
    m_result += ";false:";
    node.false_().visit(*this);

    m_result += '}';
  }

  void visit(const designated_initializers_node& node) override
  {
    m_result += "designated initializers{";

    for (const auto& n : node.initializers()) {
      m_result += "initializer{";
      n.initialization->visit(*this);
      m_result += '}';
    }

    m_result += '}';
  }

  std::string result() const { return m_result; }

private:
  std::string m_result;
};

MATCHER_P(AstEq, expected_ast, "")
{
  ast_tree_representation_visitor result_visitor;
  arg->visit(result_visitor);

  ast_tree_representation_visitor expected_visitor;
  expected_ast->visit(expected_visitor);

  *result_listener << "result: " << result_visitor.result()
                   << "\nexpected: " << expected_visitor.result();

  return result_visitor.result() == expected_visitor.result();
}

class ParserTest : public ::testing::Test
{
protected:
  auto create_qualified_name(token_t name) const
  {
    return std::vector<ast::name_with_coloncolon>{ { name } };
  }

  auto create_id_node_ptr(token_t name) const
  {
    return std::make_unique<id_node>(create_qualified_name(name));
  }
};

TEST_F(ParserTest, Factor_TrueKeywordToken_GetBoolFundamentalValue)
{
  const auto token = token_kw_true();

  auto expected_ast = std::make_unique<bool_value_node>(token);

  const auto tokens = tokens_container_t{ token };
  auto parser =
    parser_t{ dummy_err_observer, cmsl::source_view{ "" }, tokens };
  auto result_ast = parser.parse_factor();

  ASSERT_THAT(result_ast, NotNull());
  EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
}

TEST_F(ParserTest, Factor_FalseKeywordToken_GetBoolFundamentalValue)
{
  const auto token = token_kw_false();

  auto expected_ast = std::make_unique<bool_value_node>(token);

  const auto tokens = tokens_container_t{ token };
  auto parser =
    parser_t{ dummy_err_observer, cmsl::source_view{ "" }, tokens };
  auto result_ast = parser.parse_factor();

  ASSERT_THAT(result_ast, NotNull());
  EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
}

TEST_F(ParserTest, Factor_IntegerToken_GetIntFundamentalValue)
{
  const auto token = token_integer("1");

  auto expected_ast = std::make_unique<int_value_node>(token);

  const auto tokens = tokens_container_t{ token };
  auto parser =
    parser_t{ dummy_err_observer, cmsl::source_view{ "" }, tokens };
  auto result_ast = parser.parse_factor();

  ASSERT_THAT(result_ast, NotNull());
  EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
}

TEST_F(ParserTest, Factor_DoubleToken_GetDoubleFundamentalValue)
{
  const auto token = token_double("1.6");

  auto expected_ast = std::make_unique<double_value_node>(token);

  const auto tokens = tokens_container_t{ token };
  auto parser =
    parser_t{ dummy_err_observer, cmsl::source_view{ "" }, tokens };
  auto result_ast = parser.parse_factor();

  ASSERT_THAT(result_ast, NotNull());
  EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
}

TEST_F(ParserTest, Factor_StringToken_GetStringFundamentalValue)
{
  const auto token = token_string("some string");

  auto expected_ast = std::make_unique<string_value_node>(token);

  const auto tokens = tokens_container_t{ token };
  auto parser =
    parser_t{ dummy_err_observer, cmsl::source_view{ "" }, tokens };
  auto result_ast = parser.parse_factor();

  ASSERT_THAT(result_ast, NotNull());
  EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
}

TEST_F(ParserTest, Factor_IdentifierToken_GetId)
{
  const auto token = token_identifier("some_name");

  auto expected_ast = std::make_unique<id_node>(create_qualified_name(token));

  const auto tokens = tokens_container_t{ token };
  auto parser =
    parser_t{ dummy_err_observer, cmsl::source_view{ "" }, tokens };
  auto result_ast = parser.parse_factor();

  ASSERT_THAT(result_ast, NotNull());
  EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
}

TEST_F(ParserTest, Factor_ParenthesizedToken_GetInnerNode)
{
  const auto lparen = token_open_paren();
  const auto id = token_identifier("some_name");
  const auto rparen = token_close_paren();

  auto names = id_node::names_t{ { id, {} } };
  auto expected_ast = std::make_unique<id_node>(names);

  const auto tokens = tokens_container_t{ lparen, id, rparen };
  auto parser =
    parser_t{ dummy_err_observer, cmsl::source_view{ "" }, tokens };
  auto result_ast = parser.parse_factor();

  ASSERT_THAT(result_ast, NotNull());
  EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
}

TEST_F(ParserTest, Expr_TokenBinaryOpToken_GetBinaryOperator)
{
  // 1 + 2
  const auto lhs_token = token_integer("1");
  const auto op_token = token_plus();
  const auto rhs_token = token_integer("2");

  auto lhs_node = std::make_unique<int_value_node>(lhs_token);
  auto rhs_node = std::make_unique<int_value_node>(rhs_token);
  auto expected_ast = std::make_unique<binary_operator_node>(
    std::move(lhs_node), op_token, std::move(rhs_node));

  const auto tokens = tokens_container_t{ lhs_token, op_token, rhs_token };
  auto parser =
    parser_t{ dummy_err_observer, cmsl::source_view{ "" }, tokens };
  auto result_ast = parser.parse_expr();

  ASSERT_THAT(result_ast, NotNull());
  EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
}

TEST_F(ParserTest, Expr_TokenBinaryOpTokenBinaryOpToken_GetTree)
{
  // "foo" - bar - 4.2
  const auto lhs_lhs_token = token_string("foo");
  const auto lhs_op_token = token_minus();
  const auto lhs_rhs_token = token_identifier("bar");
  const auto op_token = token_minus();
  const auto rhs_token = token_double("4.2");

  auto lhs_lhs_node = std::make_unique<string_value_node>(lhs_lhs_token);
  // Todo: Introduce fixture with utils
  auto names = id_node::names_t{ { lhs_rhs_token } };
  auto lhs_rhs_node = std::make_unique<id_node>(names);
  auto lhs_node = std::make_unique<binary_operator_node>(
    std::move(lhs_lhs_node), lhs_op_token, std::move(lhs_rhs_node));

  auto rhs_node = std::make_unique<double_value_node>(rhs_token);

  auto expected_ast = std::make_unique<binary_operator_node>(
    std::move(lhs_node), op_token, std::move(rhs_node));

  const auto tokens = tokens_container_t{
    // clang-format off
    lhs_lhs_token,
    lhs_op_token,
    lhs_rhs_token,
    op_token,
    rhs_token
    // clang-format on
  };
  auto parser =
    parser_t{ dummy_err_observer, cmsl::source_view{ "" }, tokens };
  auto result_ast = parser.parse_expr();

  ASSERT_THAT(result_ast, NotNull());
  EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
}

TEST_F(ParserTest, Expr_TokenLowPrecedenceOpTokenHighPrecedenceOpToken_GetTree)
{
  // "foo" - bar * 4.2
  const auto lhs_token = token_string("foo");
  const auto op_token = token_minus();
  const auto rhs_lhs_token = token_identifier("bar");
  const auto rhs_op_token = token_star();
  const auto rhs_rhs_token = token_double("4.2");

  auto lhs_node = std::make_unique<string_value_node>(lhs_token);

  auto rhs_lhs_node =
    std::make_unique<id_node>(create_qualified_name(rhs_lhs_token));
  auto rhs_rhs_node = std::make_unique<double_value_node>(rhs_rhs_token);
  auto rhs_node = std::make_unique<binary_operator_node>(
    std::move(rhs_lhs_node), rhs_op_token, std::move(rhs_rhs_node));

  auto expected_ast = std::make_unique<binary_operator_node>(
    std::move(lhs_node), op_token, std::move(rhs_node));

  const auto tokens = tokens_container_t{
    // clang-format off
    lhs_token,
    op_token,
    rhs_lhs_token,
    rhs_op_token,
    rhs_rhs_token
    // clang-format on
  };
  auto parser =
    parser_t{ dummy_err_observer, cmsl::source_view{ "" }, tokens };
  auto result_ast = parser.parse_expr();

  ASSERT_THAT(result_ast, NotNull());
  EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
}

TEST_F(ParserTest, Expr_IdOpenParenCloseParen_GetFunctionCallWithoutParameters)
{
  // foo()
  const auto fun_name_token = token_identifier("foo");

  auto expected_ast = std::make_unique<function_call_node>(
    fun_name_token, tmp_token, function_call_node::params_t{}, tmp_token);

  const auto tokens = tokens_container_t{ fun_name_token, token_open_paren(),
                                          token_close_paren() };
  auto parser =
    parser_t{ dummy_err_observer, cmsl::source_view{ "" }, tokens };
  auto result_ast = parser.parse_expr();

  ASSERT_THAT(result_ast, NotNull());
  EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
}

TEST_F(ParserTest,
       Expr_IdOpenParenParametersCloseParen_GetFunctionCallWithParameters)
{
  // foo(bar, "baz", 42)
  const auto fun_name_token = token_identifier("foo");
  const auto param1 = token_identifier("bar");
  const auto param2 = token_string("baz");
  const auto param3 = token_integer("42");
  const auto comma = token_comma();

  function_call_node::params_t params;
  params.emplace_back(
    std::make_unique<id_node>(create_qualified_name(param1)));
  params.emplace_back(std::make_unique<string_value_node>(param2));
  params.emplace_back(std::make_unique<int_value_node>(param3));

  auto expected_ast = std::make_unique<function_call_node>(
    fun_name_token, tmp_token, std::move(params), tmp_token);

  const auto tokens = tokens_container_t{
    // clang-format off
    fun_name_token,
    token_open_paren(),
    param1,
    comma,
    param2,
    comma,
    param3,
    token_close_paren()
    // clang-format on
  };
  auto parser =
    parser_t{ dummy_err_observer, cmsl::source_view{ "" }, tokens };
  auto result_ast = parser.parse_expr();

  ASSERT_THAT(result_ast, NotNull());
  EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
}

TEST_F(ParserTest,
       Expr_IdDotIdOpenParenCloseParen_GetMemberFunctionCallWithoutParameters)
{
  // foo.bar()
  const auto class_name_token = token_identifier("foo");
  const auto fun_name_token = token_identifier("bar");

  auto lhs =
    std::make_unique<id_node>(create_qualified_name(class_name_token));

  auto expected_ast = std::make_unique<member_function_call_node>(
    std::move(lhs), tmp_token, fun_name_token, tmp_token,
    function_call_node::params_t{}, tmp_token);

  const auto tokens = tokens_container_t{
    // clang-format off
    class_name_token,
    token_dot(),
    fun_name_token,
    token_open_paren(),
    token_close_paren()
    // clang-format on
  };
  auto parser =
    parser_t{ dummy_err_observer, cmsl::source_view{ "" }, tokens };
  auto result_ast = parser.parse_expr();

  ASSERT_THAT(result_ast, NotNull());
  EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
}

TEST_F(
  ParserTest,
  Expr_IdDotIdOpenParenParametersCloseParen_GetMemberFunctionCallWithParameters)
{
  // foo.bar(baz, "qux", 42)
  const auto class_name_token = token_identifier("foo");
  const auto fun_name_token = token_identifier("bar");
  const auto param1 = token_identifier("baz");
  const auto param2 = token_string("qux");
  const auto param3 = token_integer("42");
  const auto comma = token_comma();

  auto lhs =
    std::make_unique<id_node>(create_qualified_name(class_name_token));

  function_call_node::params_t params;
  params.emplace_back(
    std::make_unique<id_node>(create_qualified_name(param1)));
  params.emplace_back(std::make_unique<string_value_node>(param2));
  params.emplace_back(std::make_unique<int_value_node>(param3));

  auto expected_ast = std::make_unique<member_function_call_node>(
    std::move(lhs), tmp_token, fun_name_token, tmp_token, std::move(params),
    tmp_token);

  const auto tokens = tokens_container_t{
    // clang-format off
    class_name_token,
    token_dot(),
    fun_name_token,
    token_open_paren(),
    param1,
    comma,
    param2,
    comma,
    param3,
    token_close_paren()
    // clang-format on
  };
  auto parser =
    parser_t{ dummy_err_observer, cmsl::source_view{ "" }, tokens };
  auto result_ast = parser.parse_expr();

  ASSERT_THAT(result_ast, NotNull());
  EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
}

TEST_F(
  ParserTest,
  Expr_ParenthesizedExpressionDotIdOpenParenCloseParen_GetMemberFunctionCallWithoutParameters)
{
  // (foo+bar).baz()
  const auto lhs_lhs_token = token_identifier("foo");
  const auto lhs_op_token = token_plus();
  const auto lhs_rhs_token = token_identifier("bar");
  const auto fun_name_token = token_identifier("baz");

  auto lhs_lhs_node =
    std::make_unique<id_node>(create_qualified_name(lhs_lhs_token));
  auto lhs_rhs_node =
    std::make_unique<id_node>(create_qualified_name(lhs_rhs_token));
  auto lhs_node = std::make_unique<binary_operator_node>(
    std::move(lhs_lhs_node), lhs_op_token, std::move(lhs_rhs_node));

  auto expected_ast = std::make_unique<member_function_call_node>(
    std::move(lhs_node), tmp_token, fun_name_token, tmp_token,
    function_call_node::params_t{}, tmp_token);

  const auto tokens = tokens_container_t{
    // clang-format off
    token_open_paren(),
    lhs_lhs_token,
    lhs_op_token,
    lhs_rhs_token,
    token_close_paren(),
    token_dot(),
    fun_name_token,
    token_open_paren(),
    token_close_paren()
    // clang-format on
  };
  auto parser =
    parser_t{ dummy_err_observer, cmsl::source_view{ "" }, tokens };
  auto result_ast = parser.parse_expr();

  ASSERT_THAT(result_ast, NotNull());
  EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
}

TEST_F(
  ParserTest,
  Expr_ParenthesizedExpressionDotIdOpenParenParamsCloseParen_GetMemberFunctionCallWithParameters)
{
  // (foo+bar).baz(baz, "qux", 42)
  const auto lhs_lhs_token = token_identifier("foo");
  const auto lhs_op_token = token_plus();
  const auto lhs_rhs_token = token_identifier("bar");
  const auto fun_name_token = token_identifier("baz");
  const auto param1 = token_identifier("baz");
  const auto param2 = token_string("qux");
  const auto param3 = token_integer("42");
  const auto comma = token_comma();

  auto lhs_lhs_node =
    std::make_unique<id_node>(create_qualified_name(lhs_lhs_token));
  auto lhs_rhs_node =
    std::make_unique<id_node>(create_qualified_name(lhs_rhs_token));
  auto lhs_node = std::make_unique<binary_operator_node>(
    std::move(lhs_lhs_node), lhs_op_token, std::move(lhs_rhs_node));
  function_call_node::params_t params;
  params.emplace_back(
    std::make_unique<id_node>(create_qualified_name(param1)));
  params.emplace_back(std::make_unique<string_value_node>(param2));
  params.emplace_back(std::make_unique<int_value_node>(param3));

  auto expected_ast = std::make_unique<member_function_call_node>(
    std::move(lhs_node), tmp_token, fun_name_token, tmp_token,
    std::move(params), tmp_token);

  const auto tokens = tokens_container_t{
    // clang-format off
    token_open_paren(),
    lhs_lhs_token,
    lhs_op_token,
    lhs_rhs_token,
    token_close_paren(),
    token_dot(),
    fun_name_token,
    token_open_paren(),
    param1,
    comma,
    param2,
    comma,
    param3,
    token_close_paren()
    // clang-format on
  };
  auto parser =
    parser_t{ dummy_err_observer, cmsl::source_view{ "" }, tokens };
  auto result_ast = parser.parse_expr();

  ASSERT_THAT(result_ast, NotNull());
  EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
}

TEST_F(ParserTest, Expr_IdDotId_GetClassMemberAccess)
{
  // foo.bar
  const auto class_name_token = token_identifier("foo");
  const auto member_name_token = token_identifier("bar");

  auto lhs_node =
    std::make_unique<id_node>(create_qualified_name(class_name_token));

  auto expected_ast = std::make_unique<class_member_access_node>(
    std::move(lhs_node), tmp_token, member_name_token);

  const auto tokens =
    tokens_container_t{ class_name_token, token_dot(), member_name_token };
  auto parser =
    parser_t{ dummy_err_observer, cmsl::source_view{ "" }, tokens };
  auto result_ast = parser.parse_expr();

  ASSERT_THAT(result_ast, NotNull());
  EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
}

TEST_F(ParserTest, Expr_ParenthesizedExpressionDotId_GetClassMemberAccess)
{
  // (foo + bar).baz
  const auto lhs_lhs_token = token_identifier("foo");
  const auto lhs_op_token = token_plus();
  const auto lhs_rhs_token = token_identifier("bar");
  const auto member_name_token = token_identifier("baz");

  auto lhs_lhs_node =
    std::make_unique<id_node>(create_qualified_name(lhs_lhs_token));
  auto lhs_rhs_node =
    std::make_unique<id_node>(create_qualified_name(lhs_rhs_token));
  auto lhs_node = std::make_unique<binary_operator_node>(
    std::move(lhs_lhs_node), lhs_op_token, std::move(lhs_rhs_node));

  auto expected_ast = std::make_unique<class_member_access_node>(
    std::move(lhs_node), tmp_token, member_name_token);

  const auto tokens = tokens_container_t{
    // clang-format off
    token_open_paren(),
    lhs_lhs_token,
    lhs_op_token,
    lhs_rhs_token,
    token_close_paren(),
    token_dot(),
    member_name_token
    // clang-format on
  };
  auto parser =
    parser_t{ dummy_err_observer, cmsl::source_view{ "" }, tokens };
  auto result_ast = parser.parse_expr();

  ASSERT_THAT(result_ast, NotNull());
  EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
}

TEST_F(ParserTest, If_IfConditionBlock_GetIfElse)
{
  // if(foo) {}
  const auto condition_token = token_identifier("foo");

  auto condition_expr_node =
    std::make_unique<id_node>(create_qualified_name(condition_token));
  auto block =
    std::make_unique<block_node>(tmp_token, block_node::nodes_t{}, tmp_token);
  auto condition_node = std::make_unique<conditional_node>(
    tmp_token, std::move(condition_expr_node), tmp_token, std::move(block));

  if_else_node::ifs_t ifs;
  ifs.emplace_back(if_else_node::if_values{ std::nullopt, tmp_token,
                                            std::move(condition_node) });

  auto expected_ast =
    std::make_unique<if_else_node>(std::move(ifs), std::nullopt);

  const auto tokens = tokens_container_t{
    // clang-format off
    token_kw_if(),
    token_open_paren(),
    condition_token,
    token_close_paren(),
    token_open_brace(),
    token_close_brace()
    // clang-format on
  };
  auto parser =
    parser_t{ dummy_err_observer, cmsl::source_view{ "" }, tokens };
  auto result_ast = parser.parse_if_else_node();

  ASSERT_THAT(result_ast, NotNull());
  EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
}

TEST_F(ParserTest, If_IfConditionParenBlockElseBlock_GetIfElse)
{
  // if(foo) {} else {}
  const auto condition_token = token_identifier("foo");

  auto condition_expr_node =
    std::make_unique<id_node>(create_qualified_name(condition_token));
  auto block =
    std::make_unique<block_node>(tmp_token, block_node::nodes_t{}, tmp_token);
  auto condition_node = std::make_unique<conditional_node>(
    tmp_token, std::move(condition_expr_node), tmp_token, std::move(block));

  if_else_node::ifs_t ifs;
  ifs.emplace_back(if_else_node::if_values{ std::nullopt, tmp_token,
                                            std::move(condition_node) });

  auto else_node =
    std::make_unique<block_node>(tmp_token, block_node::nodes_t{}, tmp_token);
  auto else_value =
    if_else_node::last_else_value{ tmp_token, std::move(else_node) };

  auto expected_ast =
    std::make_unique<if_else_node>(std::move(ifs), std::move(else_value));

  const auto tokens = tokens_container_t{
    // clang-format off
    token_kw_if(),
    token_open_paren(),
    condition_token,
    token_close_paren(),
    token_open_brace(),
    token_close_brace(),
    token_kw_else(),
    token_open_brace(),
    token_close_brace()
    // clang-format on
  };
  auto parser =
    parser_t{ dummy_err_observer, cmsl::source_view{ "" }, tokens };
  auto result_ast = parser.parse_if_else_node();

  ASSERT_THAT(result_ast, NotNull());
  EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
}

TEST_F(ParserTest, If_IfConditionBlockElseIfConditionBlockElseBlock_GetIfElse)
{
  // if(foo) {} else if(bar) else {}
  const auto if_condition_token = token_identifier("foo");
  const auto elseif_condition_token = token_identifier("bar");

  auto if_condition_expr_node =
    std::make_unique<id_node>(create_qualified_name(if_condition_token));
  auto if_block =
    std::make_unique<block_node>(tmp_token, block_node::nodes_t{}, tmp_token);
  auto if_condition_node = std::make_unique<conditional_node>(
    tmp_token, std::move(if_condition_expr_node), tmp_token,
    std::move(if_block));

  auto elseif_condition_expr_node =
    std::make_unique<id_node>(create_qualified_name(elseif_condition_token));
  auto elseif_block =
    std::make_unique<block_node>(tmp_token, block_node::nodes_t{}, tmp_token);
  auto elseif_condition_node = std::make_unique<conditional_node>(
    tmp_token, std::move(elseif_condition_expr_node), tmp_token,
    std::move(elseif_block));

  if_else_node::ifs_t ifs;
  ifs.emplace_back(if_else_node::if_values{ std::nullopt, tmp_token,
                                            std::move(if_condition_node) });
  ifs.emplace_back(if_else_node::if_values{
    tmp_token, tmp_token, std::move(elseif_condition_node) });

  auto else_node =
    std::make_unique<block_node>(tmp_token, block_node::nodes_t{}, tmp_token);
  auto else_value =
    if_else_node::last_else_value{ tmp_token, std::move(else_node) };

  auto expected_ast =
    std::make_unique<if_else_node>(std::move(ifs), std::move(else_value));

  const auto tokens = tokens_container_t{
    // clang-format off
    token_kw_if(),
    token_open_paren(),
    if_condition_token,
    token_close_paren(),
    token_open_brace(),
    token_close_brace(),

    token_kw_else(),
    token_kw_if(),
    token_open_paren(),
    elseif_condition_token,
    token_close_paren(),
    token_open_brace(),
    token_close_brace(),

    token_kw_else(),
    token_open_brace(),
    token_close_brace()
    // clang-format on
  };
  auto parser =
    parser_t{ dummy_err_observer, cmsl::source_view{ "" }, tokens };
  auto result_ast = parser.parse_if_else_node();

  ASSERT_THAT(result_ast, NotNull());
  EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
}

TEST_F(ParserTest, If_IfConditionBlockElseIfConditionBlock_GetIfElse)
{
  // if(foo) {} else if(bar)
  const auto if_condition_token = token_identifier("foo");
  const auto elseif_condition_token = token_identifier("bar");

  auto if_condition_expr_node =
    std::make_unique<id_node>(create_qualified_name(if_condition_token));
  auto if_block = std::make_unique<block_node>(
    tmp_token, block_node::nodes_t{},
    tmp_token); // Todo: introduce a fixture and method to create empty block
                // node
  auto if_condition_node = std::make_unique<conditional_node>(
    tmp_token, std::move(if_condition_expr_node), tmp_token,
    std::move(if_block));

  auto elseif_condition_expr_node =
    std::make_unique<id_node>(create_qualified_name(elseif_condition_token));
  auto elseif_block =
    std::make_unique<block_node>(tmp_token, block_node::nodes_t{}, tmp_token);
  auto elseif_condition_node = std::make_unique<conditional_node>(
    tmp_token, std::move(elseif_condition_expr_node), tmp_token,
    std::move(elseif_block));

  if_else_node::ifs_t ifs;
  ifs.emplace_back(if_else_node::if_values{ std::nullopt, tmp_token,
                                            std::move(if_condition_node) });
  ifs.emplace_back(if_else_node::if_values{
    tmp_token, tmp_token, std::move(elseif_condition_node) });

  auto expected_ast =
    std::make_unique<if_else_node>(std::move(ifs), std::nullopt);

  const auto tokens = tokens_container_t{
    // clang-format off
    token_kw_if(),
    token_open_paren(),
    if_condition_token,
    token_close_paren(),
    token_open_brace(),
    token_close_brace(),

    token_kw_else(),
    token_kw_if(),
    token_open_paren(),
    elseif_condition_token,
    token_close_paren(),
    token_open_brace(),
    token_close_brace()
    // clang-format on
  };
  auto parser =
    parser_t{ dummy_err_observer, cmsl::source_view{ "" }, tokens };
  auto result_ast = parser.parse_if_else_node();

  ASSERT_THAT(result_ast, NotNull());
  EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
}

TEST_F(ParserTest, While_WhileConditionBlock_GetWhile)
{
  // while(foo) {}
  const auto condition_token = token_identifier("foo");

  auto condition_expr_node =
    std::make_unique<id_node>(create_qualified_name(condition_token));
  auto block =
    std::make_unique<block_node>(tmp_token, block_node::nodes_t{}, tmp_token);
  auto condition_node = std::make_unique<conditional_node>(
    tmp_token, std::move(condition_expr_node), tmp_token, std::move(block));

  auto expected_ast =
    std::make_unique<while_node>(tmp_token, std::move(condition_node));

  const auto tokens = tokens_container_t{
    // clang-format off
    token_kw_while(),
    token_open_paren(),
    condition_token,
    token_close_paren(),
    token_open_brace(),
    token_close_brace()
    // clang-format on
  };
  auto parser =
    parser_t{ dummy_err_observer, cmsl::source_view{ "" }, tokens };
  auto result_ast = parser.parse_while_node();

  ASSERT_THAT(result_ast, NotNull());
  EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
}

TEST_F(ParserTest, Return_ReturnExpression_GetReturn)
{
  // return foo;
  const auto expression_token = token_identifier("foo");

  auto expr_node =
    std::make_unique<id_node>(create_qualified_name(expression_token));
  auto expected_ast =
    std::make_unique<return_node>(tmp_token, std::move(expr_node), tmp_token);

  const auto tokens = tokens_container_t{ token_kw_return(), expression_token,
                                          token_semicolon() };
  auto parser =
    parser_t{ dummy_err_observer, cmsl::source_view{ "" }, tokens };
  auto result_ast = parser.parse_return_node();

  ASSERT_THAT(result_ast, NotNull());
  EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
}

namespace type {
using Type_SingleToken = ::testing::TestWithParam<token_t>;

TEST_P(Type_SingleToken, GetTypeReference)
{
  const auto token = token_kw_int();

  const auto expected_reference =
    type_representation{ ast::qualified_name{ token } };

  const auto tokens = tokens_container_t{ token };
  auto parser =
    parser_t{ dummy_err_observer, cmsl::source_view{ "" }, tokens };
  auto result_type_reference = parser.parse_type();

  ASSERT_TRUE(result_type_reference);
  EXPECT_THAT(result_type_reference->tokens(),
              Eq(expected_reference.tokens()));
}

const auto values = ::testing::Values(
  token_kw_bool(), token_kw_int(), token_kw_double(), token_kw_string(),
  token_kw_project(), token_kw_library(), token_kw_executable(),
  token_kw_auto(), token_kw_void(), token_identifier("foo"));
INSTANTIATE_TEST_CASE_P(ParserTest, Type_SingleToken, values);
}

TEST_F(ParserTest, Type_GenericType_GetTypeReference)
{
  cmsl::string_view source = "list<int>";
  const auto list_token =
    token_from_larger_source(source, token_type_t::kw_list, 0u, 4u);
  const auto less_token =
    token_from_larger_source(source, token_type_t::less, 4u, 5u);
  const auto int_token =
    token_from_larger_source(source, token_type_t::kw_int, 5u, 8u);
  const auto greater_token =
    token_from_larger_source(source, token_type_t::greater, 8u, 9u);

  const auto value_type_representation =
    type_representation{ ast::qualified_name{ int_token } };
  const auto expected_type_tokens =
    tokens_container_t{ list_token, less_token, int_token, greater_token };
  auto generic_name =
    type_representation::generic_type_name{ expected_type_tokens,
                                            { value_type_representation } };
  const auto expected_reference =
    type_representation{ std::move(generic_name) };

  auto parser = parser_t{ dummy_err_observer, cmsl::source_view{ "" },
                          expected_type_tokens };
  auto result_type_reference = parser.parse_type();

  ASSERT_TRUE(result_type_reference);
  EXPECT_THAT(*result_type_reference, Eq(expected_reference));
}

TEST_F(ParserTest, Type_NestedGenericType_GetTypeReference)
{
  cmsl::string_view source = "list<list<foo>>";
  const auto list_token =
    token_from_larger_source(source, token_type_t::kw_list, 0u, 4u);
  const auto less_token =
    token_from_larger_source(source, token_type_t::less, 4u, 5u);
  const auto nested_list_token =
    token_from_larger_source(source, token_type_t::kw_list, 5u, 9u);
  const auto nested_less_token =
    token_from_larger_source(source, token_type_t::less, 9u, 10u);
  const auto foo_token =
    token_from_larger_source(source, token_type_t::identifier, 10u, 13u);
  const auto nested_greater_token =
    token_from_larger_source(source, token_type_t::greater, 13u, 14u);
  const auto greater_token =
    token_from_larger_source(source, token_type_t::greater, 14u, 15u);

  // foo
  const auto nested_value_type_representation =
    type_representation{ ast::qualified_name{ foo_token } };

  // list<foo>
  auto list_foo_generic_name = type_representation::generic_type_name{
    { nested_list_token, nested_less_token, foo_token, nested_greater_token },
    { nested_value_type_representation }
  };
  const auto value_type_representation =
    type_representation{ std::move(list_foo_generic_name) };

  // list<list<foo>>
  const auto expected_type_tokens = tokens_container_t{
    // clang-format off
    list_token,
    less_token,
    nested_list_token,
    nested_less_token,
    foo_token,
    nested_greater_token,
    greater_token
    // clang-format on
  };
  auto list_list_foo_generic_name =
    type_representation::generic_type_name{ expected_type_tokens,
                                            { value_type_representation } };
  const auto expected_representation =
    type_representation{ std::move(list_list_foo_generic_name) };

  auto parser = parser_t{ dummy_err_observer, cmsl::source_view{ "" },
                          expected_type_tokens };
  auto result_type_reference = parser.parse_type();

  ASSERT_TRUE(result_type_reference);
  EXPECT_THAT(*result_type_reference, Eq(expected_representation));
}

TEST_F(ParserTest, VariableDeclaration_TypeId_GetVariableDeclaration)
{
  // int foo;
  const auto type_token = token_kw_int();
  const auto name_token = token_identifier("foo");

  const auto type_repr =
    type_representation{ ast::qualified_name{ type_token } };

  auto variable_decl = std::make_unique<variable_declaration_node>(
    type_repr, name_token, std::nullopt);
  auto expected_ast = std::make_unique<standalone_variable_declaration_node>(
    std::move(variable_decl), tmp_token);

  const auto tokens =
    tokens_container_t{ type_token, name_token, token_semicolon() };
  auto parser =
    parser_t{ dummy_err_observer, cmsl::source_view{ "" }, tokens };
  auto result_ast = parser.parse_standalone_variable_declaration();

  ASSERT_THAT(result_ast, NotNull());
  EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
}

TEST_F(ParserTest, VariableDeclaration_GenericTypeId_GetVariableDeclaration)
{
  // list<int> foo;
  cmsl::string_view source = "list<int>";
  const auto list_token =
    token_from_larger_source(source, token_type_t::kw_list, 0u, 4u);
  const auto less_token =
    token_from_larger_source(source, token_type_t::less, 4u, 5u);
  const auto int_token =
    token_from_larger_source(source, token_type_t::kw_int, 5u, 8u);
  const auto greater_token =
    token_from_larger_source(source, token_type_t::greater, 8u, 9u);
  const auto name_token = token_identifier("foo");

  const auto value_type_representation =
    type_representation{ ast::qualified_name{ int_token } };
  const auto representation_tokens =
    tokens_container_t{ list_token, less_token, int_token, greater_token };
  auto generic_name =
    type_representation::generic_type_name{ representation_tokens,
                                            { value_type_representation } };
  const auto representation = type_representation{ std::move(generic_name) };

  auto variable_decl = std::make_unique<variable_declaration_node>(
    representation, name_token, std::nullopt);
  auto expected_ast = std::make_unique<standalone_variable_declaration_node>(
    std::move(variable_decl), tmp_token);

  const auto tokens = tokens_container_t{
    // clang-format off
    list_token,
    less_token,
    int_token,
    greater_token,
    name_token,
    token_semicolon()
    // clang-format on
  };
  auto parser =
    parser_t{ dummy_err_observer, cmsl::source_view{ "" }, tokens };
  auto result_ast = parser.parse_standalone_variable_declaration();

  ASSERT_THAT(result_ast, NotNull());
  EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
}

TEST_F(ParserTest,
       VariableDeclaration_TypeIdEqualExpression_GetVariableDeclaration)
{
  // int foo = 42;
  const auto type_token = token_kw_int();
  const auto name_token = token_identifier("foo");
  const auto int_expr_token = token_integer("42");

  const auto representation =
    type_representation{ ast::qualified_name{ type_token } };

  auto expr = std::make_unique<int_value_node>(int_expr_token);
  auto initialization =
    standalone_variable_declaration_node::initialization_values_t{
      tmp_token, std::move(expr)
    };

  auto variable_decl = std::make_unique<variable_declaration_node>(
    representation, name_token, std::move(initialization));
  auto expected_ast = std::make_unique<standalone_variable_declaration_node>(
    std::move(variable_decl), tmp_token);

  const auto tokens = tokens_container_t{
    // clang-format off
    type_token,
    name_token,
    token_equal(),
    int_expr_token,
    token_semicolon()
    // clang-format on
  };
  auto parser =
    parser_t{ dummy_err_observer, cmsl::source_view{ "" }, tokens };
  auto result_ast = parser.parse_standalone_variable_declaration();

  ASSERT_THAT(result_ast, NotNull());
  EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
}

TEST_F(ParserTest,
       VariableDeclaration_GenericTypeIdEqualExpression_GetVariableDeclaration)
{
  // list<int> foo = 42;
  cmsl::string_view source = "list<int>";
  const auto list_token =
    token_from_larger_source(source, token_type_t::kw_list, 0u, 4u);
  const auto less_token =
    token_from_larger_source(source, token_type_t::less, 4u, 5u);
  const auto int_token =
    token_from_larger_source(source, token_type_t::kw_int, 5u, 8u);
  const auto greater_token =
    token_from_larger_source(source, token_type_t::greater, 8u, 9u);
  const auto name_token = token_identifier("foo");
  const auto int_expr_token = token_integer("42");

  const auto value_type_representation =
    type_representation{ ast::qualified_name{ int_token } };
  const auto representation_tokens =
    tokens_container_t{ list_token, less_token, int_token, greater_token };
  auto generic_name =
    type_representation::generic_type_name{ representation_tokens,
                                            { value_type_representation } };
  const auto representation = type_representation{ std::move(generic_name) };

  auto expr = std::make_unique<int_value_node>(int_expr_token);
  auto initialization =
    standalone_variable_declaration_node::initialization_values_t{
      tmp_token, std::move(expr)
    };

  auto variable_decl = std::make_unique<variable_declaration_node>(
    representation, name_token, std::move(initialization));
  auto expected_ast = std::make_unique<standalone_variable_declaration_node>(
    std::move(variable_decl), tmp_token);

  const auto tokens = tokens_container_t{
    // clang-format off
    list_token,
    less_token,
    int_token,
    greater_token,
    name_token,
    token_equal(),
    int_expr_token,
    token_semicolon()
    // clang-format on
  };
  auto parser =
    parser_t{ dummy_err_observer, cmsl::source_view{ "" }, tokens };
  auto result_ast = parser.parse_standalone_variable_declaration();

  ASSERT_THAT(result_ast, NotNull());
  EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
}

TEST_F(ParserTest, Block_OpenBraceCloseBrace_GetBlock)
{
  // {}
  auto expected_ast =
    std::make_unique<block_node>(tmp_token, block_node::nodes_t{}, tmp_token);

  const auto tokens =
    tokens_container_t{ token_open_brace(), token_close_brace() };
  auto parser =
    parser_t{ dummy_err_observer, cmsl::source_view{ "" }, tokens };
  auto result_ast = parser.parse_block();

  ASSERT_THAT(result_ast, NotNull());
  EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
}

TEST_F(ParserTest, Block_VariableDeclaration_GetBlock)
{
  // { foo bar; }
  const auto variable_type_token = token_identifier("foo");
  const auto variable_name_token = token_identifier("bar");
  const auto variable_type_ref =
    type_representation{ ast::qualified_name{ variable_type_token } };

  auto variable_decl = std::make_unique<variable_declaration_node>(
    variable_type_ref, variable_name_token, std::nullopt);
  auto variable_decl_node =
    std::make_unique<standalone_variable_declaration_node>(
      std::move(variable_decl), tmp_token);

  block_node::nodes_t exprs;
  exprs.emplace_back(std::move(variable_decl_node));

  auto expected_ast =
    std::make_unique<block_node>(tmp_token, std::move(exprs), tmp_token);

  const auto tokens = tokens_container_t{
    // clang-format off
    token_open_brace(),

    variable_type_token,
    variable_name_token,
    token_semicolon(),

    token_close_brace()
    // clang-format on
  };
  auto parser =
    parser_t{ dummy_err_observer, cmsl::source_view{ "" }, tokens };
  auto result_ast = parser.parse_block();

  ASSERT_THAT(result_ast, NotNull());
  EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
}

TEST_F(ParserTest, Block_IfElse_GetBlock)
{
  // { if(foo) {} else if(bar) else {} }
  const auto if_condition_token = token_identifier("foo");
  const auto elseif_condition_token = token_identifier("bar");

  auto if_condition_expr_node =
    std::make_unique<id_node>(create_qualified_name(if_condition_token));
  auto if_block =
    std::make_unique<block_node>(tmp_token, block_node::nodes_t{}, tmp_token);
  auto if_condition_node = std::make_unique<conditional_node>(
    tmp_token, std::move(if_condition_expr_node), tmp_token,
    std::move(if_block));

  auto elseif_condition_expr_node =
    std::make_unique<id_node>(create_qualified_name(elseif_condition_token));
  auto elseif_block =
    std::make_unique<block_node>(tmp_token, block_node::nodes_t{}, tmp_token);
  auto elseif_condition_node = std::make_unique<conditional_node>(
    tmp_token, std::move(elseif_condition_expr_node), tmp_token,
    std::move(elseif_block));

  if_else_node::ifs_t ifs;
  ifs.emplace_back(if_else_node::if_values{ std::nullopt, tmp_token,
                                            std::move(if_condition_node) });
  ifs.emplace_back(if_else_node::if_values{
    tmp_token, tmp_token, std::move(elseif_condition_node) });

  auto else_node =
    std::make_unique<block_node>(tmp_token, block_node::nodes_t{}, tmp_token);
  auto else_value =
    if_else_node::last_else_value{ tmp_token, std::move(else_node) };

  auto if_else =
    std::make_unique<if_else_node>(std::move(ifs), std::move(else_value));

  block_node::nodes_t exprs;
  exprs.emplace_back(std::move(if_else));

  auto expected_ast =
    std::make_unique<block_node>(tmp_token, std::move(exprs), tmp_token);

  const auto tokens = tokens_container_t{
    // clang-format off
    token_open_brace(),

    token_kw_if(),
    token_open_paren(),
    if_condition_token,
    token_close_paren(),
    token_open_brace(),
    token_close_brace(),

    token_kw_else(),
    token_kw_if(),
    token_open_paren(),
    elseif_condition_token,
    token_close_paren(),
    token_open_brace(),
    token_close_brace(),

    token_kw_else(),
    token_open_brace(),
    token_close_brace(),

    token_close_brace()
    // clang-format on
  };
  auto parser =
    parser_t{ dummy_err_observer, cmsl::source_view{ "" }, tokens };
  auto result_ast = parser.parse_block();

  ASSERT_THAT(result_ast, NotNull());
  EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
}

TEST_F(ParserTest, Block_Expression_GetBlock)
{
  // { foo + bar; }
  const auto lhs_token = token_integer("1");
  const auto op_token = token_plus();
  const auto rhs_token = token_integer("2");

  auto lhs_node = std::make_unique<int_value_node>(lhs_token);
  auto rhs_node = std::make_unique<int_value_node>(rhs_token);
  auto expr_node = std::make_unique<binary_operator_node>(
    std::move(lhs_node), op_token, std::move(rhs_node));

  block_node::nodes_t exprs;
  exprs.emplace_back(std::move(expr_node));

  auto expected_ast =
    std::make_unique<block_node>(tmp_token, std::move(exprs), tmp_token);

  const auto tokens = tokens_container_t{
    // clang-format off
    token_open_brace(),

    lhs_token,
    op_token,
    rhs_token,
    token_semicolon(),

    token_close_brace()
    // clang-format on
  };
  auto parser =
    parser_t{ dummy_err_observer, cmsl::source_view{ "" }, tokens };
  auto result_ast = parser.parse_block();

  ASSERT_THAT(result_ast, NotNull());
  EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
}

TEST_F(ParserTest, Block_While_GetBlock)
{
  // { while(foo) {} }
  const auto condition_token = token_identifier("foo");

  auto condition_expr_node =
    std::make_unique<id_node>(create_qualified_name(condition_token));
  auto block =
    std::make_unique<block_node>(tmp_token, block_node::nodes_t{}, tmp_token);
  auto condition_node = std::make_unique<conditional_node>(
    tmp_token, std::move(condition_expr_node), tmp_token, std::move(block));

  auto while_ast_node =
    std::make_unique<while_node>(tmp_token, std::move(condition_node));

  block_node::nodes_t exprs;
  exprs.emplace_back(std::move(while_ast_node));

  auto expected_ast =
    std::make_unique<block_node>(tmp_token, std::move(exprs), tmp_token);

  const auto tokens = tokens_container_t{
    // clang-format off
    token_open_brace(),

    token_kw_while(),
    token_open_paren(),
    condition_token,
    token_close_paren(),
    token_open_brace(),
    token_close_brace(),

    token_close_brace()
    // clang-format on
  };
  auto parser =
    parser_t{ dummy_err_observer, cmsl::source_view{ "" }, tokens };
  auto result_ast = parser.parse_block();

  ASSERT_THAT(result_ast, NotNull());
  EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
}

TEST_F(ParserTest, Block_Return_GetBlock)
{
  // { return foo; }
  const auto expression_token = token_identifier("foo");

  auto expr_node =
    std::make_unique<id_node>(create_qualified_name(expression_token));
  auto return_ast_node =
    std::make_unique<return_node>(tmp_token, std::move(expr_node), tmp_token);

  block_node::nodes_t exprs;
  exprs.emplace_back(std::move(return_ast_node));

  auto expected_ast =
    std::make_unique<block_node>(tmp_token, std::move(exprs), tmp_token);

  const auto tokens = tokens_container_t{
    // clang-format off
    token_open_brace(),
    token_kw_return(),
    expression_token,
    token_semicolon(),
    token_close_brace()
    // clang-format on
  };
  auto parser =
    parser_t{ dummy_err_observer, cmsl::source_view{ "" }, tokens };
  auto result_ast = parser.parse_block();

  ASSERT_THAT(result_ast, NotNull());
  EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
}

TEST_F(ParserTest, Block_NestedBlock_GetBlock)
{
  // { {} }

  auto nested_block_node =
    std::make_unique<block_node>(tmp_token, block_node::nodes_t{}, tmp_token);
  block_node::nodes_t exprs;
  exprs.emplace_back(std::move(nested_block_node));

  auto expected_ast =
    std::make_unique<block_node>(tmp_token, std::move(exprs), tmp_token);

  const auto tokens = tokens_container_t{
    // clang-format off
    token_open_brace(),
    token_open_brace(),
    token_close_brace(),
    token_close_brace()
    // clang-format on
  };
  auto parser =
    parser_t{ dummy_err_observer, cmsl::source_view{ "" }, tokens };
  auto result_ast = parser.parse_block();

  ASSERT_THAT(result_ast, NotNull());
  EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
}

TEST_F(ParserTest, Function_TypeIdOpenParenCloseParen_GetFunction)
{
  // double foo() {}
  const auto function_type_token = token_kw_double();
  const auto function_name_token = token_identifier("foo");
  const auto function_type_ref =
    type_representation{ ast::qualified_name{ function_type_token } };
  auto function_block_node =
    std::make_unique<block_node>(tmp_token, block_node::nodes_t{}, tmp_token);
  auto expected_ast = std::make_unique<user_function_node>(
    function_type_ref, function_name_token, tmp_token,
    user_function_node::params_t{}, tmp_token, std::move(function_block_node));

  const auto tokens = tokens_container_t{
    // clang-format off
    function_type_token,
    function_name_token,
    token_open_paren(),
    token_close_paren(),
    token_open_brace(),
    token_close_brace()
    // clang-format on
  };
  auto parser =
    parser_t{ dummy_err_observer, cmsl::source_view{ "" }, tokens };
  auto result_ast = parser.parse_function();

  ASSERT_THAT(result_ast, NotNull());
  EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
}

TEST_F(ParserTest, Function_TypeIdOpenParenParameterCloseParen_GetFunction)
{
  // double foo(bar baz) {}
  const auto function_type_token = token_kw_double();
  const auto function_name_token = token_identifier("foo");
  const auto function_type_ref =
    type_representation{ ast::qualified_name{ function_type_token } };

  const auto param_type_token = token_identifier("bar");
  const auto param_name_token = token_identifier("baz");
  const auto param_type_ref =
    type_representation{ ast::qualified_name{ param_type_token } };

  user_function_node::params_t params{ param_declaration{ param_type_ref,
                                                          param_name_token } };

  auto function_block_node =
    std::make_unique<block_node>(tmp_token, block_node::nodes_t{}, tmp_token);
  auto expected_ast = std::make_unique<user_function_node>(
    function_type_ref, function_name_token, tmp_token, std::move(params),
    tmp_token, std::move(function_block_node));

  const auto tokens = tokens_container_t{
    // clang-format off
    function_type_token,
    function_name_token,
    token_open_paren(),

    param_type_token,
    param_name_token,

    token_close_paren(),
    token_open_brace(),
    token_close_brace()
    // clang-format on
  };
  auto parser =
    parser_t{ dummy_err_observer, cmsl::source_view{ "" }, tokens };
  auto result_ast = parser.parse_function();

  ASSERT_THAT(result_ast, NotNull());
  EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
}

TEST_F(ParserTest, Function_TypeIdOpenParenParametersCloseParen_GetFunction)
{
  // double foo(bar baz, qux out_of_fancy_identifiers) {}
  const auto function_type_token = token_kw_double();
  const auto function_name_token = token_identifier("foo");
  const auto function_type_ref =
    type_representation{ ast::qualified_name{ function_type_token } };

  const auto param_type_token = token_identifier("bar");
  const auto param_name_token = token_identifier("baz");
  const auto param_type_ref =
    type_representation{ ast::qualified_name{ param_type_token } };

  const auto param2_type_token = token_identifier("qux");
  const auto param2_name_token = token_identifier("out_of_fancy_identifiers");
  const auto param2_type_ref =
    type_representation{ ast::qualified_name{ param2_type_token } };

  user_function_node::params_t params{
    param_declaration{ param_type_ref, param_name_token },
    param_declaration{ param2_type_ref, param2_name_token }
  };

  auto function_block_node =
    std::make_unique<block_node>(tmp_token, block_node::nodes_t{}, tmp_token);
  auto expected_ast = std::make_unique<user_function_node>(
    function_type_ref, function_name_token, tmp_token, std::move(params),
    tmp_token, std::move(function_block_node));

  const auto tokens = tokens_container_t{
    // clang-format off
    function_type_token,
    function_name_token,
    token_open_paren(),

    param_type_token,
    param_name_token,
    token_comma(),

    param2_type_token,
    param2_name_token,

    token_close_paren(),
    token_open_brace(),
    token_close_brace()
    // clang-format on
  };
  auto parser =
    parser_t{ dummy_err_observer, cmsl::source_view{ "" }, tokens };
  auto result_ast = parser.parse_function();

  ASSERT_THAT(result_ast, NotNull());
  EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
}

TEST_F(ParserTest, Class_ClassIdEmptyBlock_GetClass)
{
  // class foo {};
  const auto name_token = token_identifier("foo");

  auto expected_ast =
    std::make_unique<class_node>(tmp_token, name_token, tmp_token,
                                 class_node::nodes_t{}, tmp_token, tmp_token);

  const auto tokens = tokens_container_t{
    // clang-format off
    token_kw_class(),
    name_token,
    token_open_brace(),
    token_close_brace(),
    token_semicolon()
    // clang-format on
  };
  auto parser =
    parser_t{ dummy_err_observer, cmsl::source_view{ "" }, tokens };
  auto result_ast = parser.parse_class();

  ASSERT_THAT(result_ast, NotNull());
  EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
}

TEST_F(ParserTest, Class_ClassIdVariableDeclaration_GetClass)
{
  // class foo { int bar; };
  const auto name_token = token_identifier("foo");
  const auto variable_type_token = token_kw_int();
  const auto variable_name_token = token_identifier("bar");
  const auto variable_type_ref =
    type_representation{ ast::qualified_name{ variable_type_token } };

  auto variable_decl = std::make_unique<variable_declaration_node>(
    variable_type_ref, variable_name_token, std::nullopt);
  auto variable_decl_node =
    std::make_unique<standalone_variable_declaration_node>(
      std::move(variable_decl), tmp_token);

  class_node::nodes_t nodes;
  nodes.emplace_back(std::move(variable_decl_node));

  auto expected_ast = std::make_unique<class_node>(
    tmp_token, name_token, tmp_token, std::move(nodes), tmp_token, tmp_token);

  const auto tokens = tokens_container_t{
    // clang-format off
      token_kw_class(),
      name_token,
      token_open_brace(),

      variable_type_token,
      variable_name_token,
      token_semicolon(),

      token_close_brace(),
      token_semicolon()
    // clang-format on
  };
  auto parser =
    parser_t{ dummy_err_observer, cmsl::source_view{ "" }, tokens };
  auto result_ast = parser.parse_class();

  ASSERT_THAT(result_ast, NotNull());
  EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
}

TEST_F(ParserTest, Class_ClassIdFunctionVariableDeclaration_GetClass)
{
  // class foo { double bar() {} baz qux; };
  const auto name_token = token_identifier("foo");

  const auto function_type_token = token_kw_double();
  const auto function_name_token = token_identifier("bar");
  const auto function_type_ref =
    type_representation{ ast::qualified_name{ function_type_token } };
  auto function_block_node =
    std::make_unique<block_node>(tmp_token, block_node::nodes_t{}, tmp_token);
  auto fun_node = std::make_unique<user_function_node>(
    function_type_ref, function_name_token, tmp_token,
    user_function_node::params_t{}, tmp_token, std::move(function_block_node));

  const auto variable_type_token = token_identifier("baz");
  const auto variable_name_token = token_identifier("qux");
  const auto variable_type_ref =
    type_representation{ ast::qualified_name{ variable_type_token } };

  auto variable_decl = std::make_unique<variable_declaration_node>(
    variable_type_ref, variable_name_token, std::nullopt);
  auto variable_decl_node =
    std::make_unique<standalone_variable_declaration_node>(
      std::move(variable_decl), tmp_token);

  class_node::nodes_t nodes;
  nodes.emplace_back(std::move(fun_node));
  nodes.emplace_back(std::move(variable_decl_node));

  auto expected_ast = std::make_unique<class_node>(
    tmp_token, name_token, tmp_token, std::move(nodes), tmp_token, tmp_token);

  const auto tokens = tokens_container_t{
    // clang-format off
    token_kw_class(),
    name_token,
    token_open_brace(),

    function_type_token,
    function_name_token,
    token_open_paren(),
    token_close_paren(),
    token_open_brace(),
    token_close_brace(),

    variable_type_token,
    variable_name_token,
    token_semicolon(),

    token_close_brace(),
    token_semicolon()
    // clang-format on
  };
  auto parser =
    parser_t{ dummy_err_observer, cmsl::source_view{ "" }, tokens };
  auto result_ast = parser.parse_class();

  ASSERT_THAT(result_ast, NotNull());
  EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
}

TEST_F(ParserTest, Factor_InitializerList_GetInitializerList)
{
  // { foo, bar(baz, qux), { nest_foo, nest_bar } }
  const auto foo_token = token_identifier("foo");
  const auto bar_token = token_identifier("bar");
  const auto baz_token = token_identifier("baz");
  const auto qux_token = token_identifier("qux");
  const auto nest_foo_token = token_identifier("nest_foo");
  const auto nest_bar_token = token_identifier("nest_bar");
  const auto ob_token = token_open_brace();
  const auto cb_token = token_close_brace();

  auto foo = std::make_unique<id_node>(create_qualified_name(foo_token));

  auto baz = std::make_unique<id_node>(create_qualified_name(baz_token));
  auto qux = std::make_unique<id_node>(create_qualified_name(qux_token));
  std::vector<std::unique_ptr<ast_node>> bar_params;
  bar_params.emplace_back(std::move(baz));
  bar_params.emplace_back(std::move(qux));
  auto bar = std::make_unique<function_call_node>(
    bar_token, tmp_token, std::move(bar_params), tmp_token);

  auto nested_foo =
    std::make_unique<id_node>(create_qualified_name(nest_foo_token));
  auto nested_bar =
    std::make_unique<id_node>(create_qualified_name(nest_bar_token));
  std::vector<std::unique_ptr<ast_node>> nested_list_values;
  nested_list_values.emplace_back(std::move(nested_foo));
  nested_list_values.emplace_back(std::move(nested_bar));
  auto nested_list = std::make_unique<initializer_list_node>(
    ob_token, std::move(nested_list_values), cb_token);

  std::vector<std::unique_ptr<ast_node>> expected_list_values;
  expected_list_values.emplace_back(std::move(foo));
  expected_list_values.emplace_back(std::move(bar));
  expected_list_values.emplace_back(std::move(nested_list));
  auto expected_ast = std::make_unique<initializer_list_node>(
    ob_token, std::move(expected_list_values), cb_token);

  const auto tokens = tokens_container_t{
    // clang-format off
    token_open_brace(),

    foo_token,
    token_comma(),

    bar_token,
    token_open_paren(),
    baz_token,
    token_comma(),
    qux_token,
    token_close_paren(),
    token_comma(),

    token_open_brace(),
    nest_foo_token,
    token_comma(),
    nest_bar_token,
    token_close_brace(),

    token_close_brace()
    // clang-format on
  };
  auto parser =
    parser_t{ dummy_err_observer, cmsl::source_view{ "" }, tokens };
  auto result_ast = parser.parse_initializer_list();

  ASSERT_THAT(result_ast, NotNull());
  EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
}

TEST_F(ParserTest, For_EmptyInitEmptyConditionEmptyIterationEmptyBody_GetFor)
{
  // for(;;) {}
  auto body = std::make_unique<block_node>(
    token_open_brace(), block_node::nodes_t{}, token_close_brace());

  auto expected_ast = std::make_unique<for_node>(
    token_kw_for(), token_open_paren(), nullptr, token_semicolon(), nullptr,
    token_semicolon(), nullptr, token_close_paren(), std::move(body));

  const auto tokens = tokens_container_t{
    // clang-format off
    token_kw_for(),

    token_open_paren(),

    token_semicolon(),
    token_semicolon(),

    token_close_paren(),

    token_open_brace(),
    token_close_brace()
    // clang-format on
  };

  auto parser =
    parser_t{ dummy_err_observer, cmsl::source_view{ "" }, tokens };
  auto result_ast = parser.parse_for_node();

  ASSERT_THAT(result_ast, NotNull());
  EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
}

TEST_F(ParserTest,
       For_VariableDeclarationInitEmptyConditionEmptyIterationEmptyBody_GetFor)
{
  // for(int foo;;) {}
  const auto int_token = token_kw_int();
  const auto int_representation =
    type_representation{ ast::qualified_name{ int_token } };
  const auto foo_token = token_identifier("foo");

  auto foo_declaration = std::make_unique<variable_declaration_node>(
    int_representation, foo_token, std::nullopt);

  auto body = std::make_unique<block_node>(
    token_open_brace(), block_node::nodes_t{}, token_close_brace());

  auto expected_ast = std::make_unique<for_node>(
    token_kw_for(), token_open_paren(), std::move(foo_declaration),
    token_semicolon(), nullptr, token_semicolon(), nullptr,
    token_close_paren(), std::move(body));

  const auto tokens = tokens_container_t{
    // clang-format off
    token_kw_for(),

    token_open_paren(),

    int_token,
    foo_token,

    token_semicolon(),

    token_semicolon(),

    token_close_paren(),

    token_open_brace(),
    token_close_brace()
    // clang-format on
  };

  auto parser =
    parser_t{ dummy_err_observer, cmsl::source_view{ "" }, tokens };
  auto result_ast = parser.parse_for_node();

  ASSERT_THAT(result_ast, NotNull());
  EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
}

TEST_F(
  ParserTest,
  For_VariableDeclarationWithInitializationInitEmptyConditionEmptyIterationEmptyBody_GetFor)
{
  // for(int foo = 42;;) {}
  const auto int_token = token_kw_int();
  const auto int_representation =
    type_representation{ ast::qualified_name{ int_token } };
  const auto foo_token = token_identifier("foo");
  const auto equal_token = token_equal();
  const auto value_token = token_integer("42");

  auto init_values = variable_declaration_node::initialization_values{
    equal_token, std::make_unique<int_value_node>(value_token)
  };

  auto foo_declaration = std::make_unique<variable_declaration_node>(
    int_representation, foo_token, std::move(init_values));

  auto body = std::make_unique<block_node>(
    token_open_brace(), block_node::nodes_t{}, token_close_brace());

  auto expected_ast = std::make_unique<for_node>(
    token_kw_for(), token_open_paren(), std::move(foo_declaration),
    token_semicolon(), nullptr, token_semicolon(), nullptr,
    token_close_paren(), std::move(body));

  const auto tokens = tokens_container_t{
    // clang-format off
    token_kw_for(),

    token_open_paren(),

    int_token,
    foo_token,
    equal_token,
    value_token,

    token_semicolon(),

    token_semicolon(),

    token_close_paren(),

    token_open_brace(),
    token_close_brace()
    // clang-format on
  };

  auto parser =
    parser_t{ dummy_err_observer, cmsl::source_view{ "" }, tokens };
  auto result_ast = parser.parse_for_node();

  ASSERT_THAT(result_ast, NotNull());
  EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
}

TEST_F(ParserTest, For_EmptyInitWithConditionEmptyIterationEmptyBody_GetFor)
{
  // for(;true;) {}
  const auto true_token = token_kw_true();

  auto condition = std::make_unique<bool_value_node>(true_token);

  auto body = std::make_unique<block_node>(
    token_open_brace(), block_node::nodes_t{}, token_close_brace());

  auto expected_ast = std::make_unique<for_node>(
    token_kw_for(), token_open_paren(), nullptr, token_semicolon(),
    std::move(condition), token_semicolon(), nullptr, token_close_paren(),
    std::move(body));

  const auto tokens = tokens_container_t{
    // clang-format off
    token_kw_for(),

    token_open_paren(),

    token_semicolon(),

    true_token,

    token_semicolon(),

    token_close_paren(),

    token_open_brace(),
    token_close_brace()
    // clang-format on
  };

  auto parser =
    parser_t{ dummy_err_observer, cmsl::source_view{ "" }, tokens };
  auto result_ast = parser.parse_for_node();

  ASSERT_THAT(result_ast, NotNull());
  EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
}

TEST_F(ParserTest, For_EmptyInitEmptyConditionWithIterationEmptyBody_GetFor)
{
  // for(;; foo()) {}
  const auto foo_token = token_identifier("foo");

  auto iteration = std::make_unique<function_call_node>(
    foo_token, token_open_paren(), function_call_node::params_t{},
    token_close_paren());

  auto body = std::make_unique<block_node>(
    token_open_brace(), block_node::nodes_t{}, token_close_brace());

  auto expected_ast = std::make_unique<for_node>(
    token_kw_for(), token_open_paren(), nullptr, token_semicolon(), nullptr,
    token_semicolon(), std::move(iteration), token_close_paren(),
    std::move(body));

  const auto tokens = tokens_container_t{
    // clang-format off
    token_kw_for(),

    token_open_paren(),

    token_semicolon(),

    token_semicolon(),

    foo_token,
    token_open_paren(),
    token_close_paren(),

    token_close_paren(),

    token_open_brace(),
    token_close_brace()
    // clang-format on
  };

  auto parser =
    parser_t{ dummy_err_observer, cmsl::source_view{ "" }, tokens };
  auto result_ast = parser.parse_for_node();

  ASSERT_THAT(result_ast, NotNull());
  EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
}

TEST_F(ParserTest, Break_BreakSemicolon_GetBreak)
{
  // break;
  auto expected_ast =
    std::make_unique<break_node>(token_kw_break(), token_semicolon());

  const auto tokens = tokens_container_t{
    // clang-format off
    token_kw_break(),
    token_semicolon()
    // clang-format on
  };

  auto parser =
    parser_t{ dummy_err_observer, cmsl::source_view{ "" }, tokens };
  auto result_ast = parser.parse_break();

  ASSERT_THAT(result_ast, NotNull());
  EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
}

TEST_F(ParserTest, Namespace_EmptyNamespace_GetNamespace)
{
  // namespace foo {}
  const auto foo_token = token_identifier("foo");
  auto names = namespace_node::names_t{ { foo_token, std::nullopt } };

  auto expected_ast = std::make_unique<namespace_node>(
    token_kw_namespace(), std::move(names), token_open_brace(),
    namespace_node::nodes_t{}, token_close_brace());

  const auto tokens = tokens_container_t{
    // clang-format off
    token_kw_namespace(),
    foo_token,
    token_open_brace(),
    token_close_brace()
    // clang-format on
  };

  auto parser =
    parser_t{ dummy_err_observer, cmsl::source_view{ "" }, tokens };
  auto result_ast = parser.parse_namespace();

  ASSERT_THAT(result_ast, NotNull());
  EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
}

TEST_F(ParserTest, Namespace_NestedNamespace_GetNamespaceWithNestedNamespace)
{
  // namespace foo { namespace bar {} }
  const auto foo_token = token_identifier("foo");
  auto foo_names = namespace_node::names_t{ { foo_token, std::nullopt } };
  const auto bar_token = token_identifier("foo");
  auto bar_names = namespace_node::names_t{ { bar_token, std::nullopt } };

  auto nested_namespace = std::make_unique<namespace_node>(
    token_kw_namespace(), std::move(bar_names), token_open_brace(),
    namespace_node::nodes_t{}, token_close_brace());

  namespace_node::nodes_t nodes;
  nodes.emplace_back(std::move(nested_namespace));

  auto expected_ast = std::make_unique<namespace_node>(
    token_kw_namespace(), std::move(foo_names), token_open_brace(),
    std::move(nodes), token_close_brace());

  const auto tokens = tokens_container_t{
    // clang-format off
    token_kw_namespace(),
    foo_token,
    token_open_brace(),

    token_kw_namespace(),
    bar_token,
    token_open_brace(),
    token_close_brace(),

    token_close_brace()
    // clang-format on
  };

  auto parser =
    parser_t{ dummy_err_observer, cmsl::source_view{ "" }, tokens };
  auto result_ast = parser.parse_namespace();

  ASSERT_THAT(result_ast, NotNull());
  EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
}

TEST_F(ParserTest,
       Namespace_InlineNestedNamespace_GetNamespaceWithMultipleNames)
{
  // namespace foo::bar {}
  const auto foo_token = token_identifier("foo");
  const auto bar_token = token_identifier("bar");
  auto names = namespace_node::names_t{ { foo_token, token_coloncolon() },
                                        { bar_token } };

  auto expected_ast = std::make_unique<namespace_node>(
    token_kw_namespace(), std::move(names), token_open_brace(),
    namespace_node::nodes_t{}, token_close_brace());

  const auto tokens = tokens_container_t{
    // clang-format off
    token_kw_namespace(),
    foo_token,
    token_coloncolon(),
    bar_token,
    token_open_brace(),
    token_close_brace()
    // clang-format on
  };

  auto parser =
    parser_t{ dummy_err_observer, cmsl::source_view{ "" }, tokens };
  auto result_ast = parser.parse_namespace();

  ASSERT_THAT(result_ast, NotNull());
  EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
}

TEST_F(ParserTest, Expr_TernaryOperator_GetTernaryOperator)
{
  // foo ? bar : baz
  const auto foo_token = token_identifier("foo");
  const auto bar_token = token_identifier("bar");
  const auto baz_token = token_identifier("baz");

  auto foo_node = create_id_node_ptr(foo_token);
  auto bar_node = create_id_node_ptr(bar_token);
  auto baz_node = create_id_node_ptr(baz_token);

  auto expected_ast = std::make_unique<ternary_operator_node>(
    std::move(foo_node), token_question(), std::move(bar_node), token_colon(),
    std::move(baz_node));

  const auto tokens = tokens_container_t{
    // clang-format off
    foo_token,
    token_question(),
    bar_token,
    token_colon(),
    baz_token
    // clang-format on
  };

  auto parser =
    parser_t{ dummy_err_observer, cmsl::source_view{ "" }, tokens };
  auto result_ast = parser.parse_expr();

  ASSERT_THAT(result_ast, NotNull());
  EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
}

TEST_F(ParserTest, Expr_NestedTernaryOperator_GetTernaryOperator)
{
  // foo ? bar ? baz : qux : kek
  const auto foo_token = token_identifier("foo");
  const auto bar_token = token_identifier("bar");
  const auto baz_token = token_identifier("baz");
  const auto qux_token = token_identifier("qux");
  const auto kek_token = token_identifier("kek");

  auto foo_node = create_id_node_ptr(foo_token);
  auto bar_node = create_id_node_ptr(bar_token);
  auto baz_node = create_id_node_ptr(baz_token);
  auto qux_node = create_id_node_ptr(qux_token);
  auto kek_node = create_id_node_ptr(kek_token);

  auto nested_ternary = std::make_unique<ternary_operator_node>(
    std::move(bar_node), token_question(), std::move(baz_node), token_colon(),
    std::move(qux_node));

  auto expected_ast = std::make_unique<ternary_operator_node>(
    std::move(foo_node), token_question(), std::move(nested_ternary),
    token_colon(), std::move(kek_node));

  const auto tokens = tokens_container_t{
    // clang-format off
    foo_token,
    token_question(),
    bar_token,
    token_question(),
    baz_token,
    token_colon(),
    qux_token,
    token_colon(),
    kek_token
    // clang-format on
  };

  auto parser =
    parser_t{ dummy_err_observer, cmsl::source_view{ "" }, tokens };
  auto result_ast = parser.parse_expr();

  ASSERT_THAT(result_ast, NotNull());
  EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
}

TEST_F(ParserTest, DesignatedInitializers_GetDesignatedInitializers)
{
  // { .foo = bar, .baz = qux }
  const auto foo_token = token_identifier("foo");
  const auto bar_token = token_identifier("bar");
  const auto baz_token = token_identifier("baz");
  const auto qux_token = token_identifier("qux");

  auto bar_node = create_id_node_ptr(bar_token);
  auto qux_node = create_id_node_ptr(qux_token);

  designated_initializers_node::initializers_t initializers;
  initializers.emplace_back(designated_initializers_node::initializer{
    token_dot(), foo_token, token_equal(), std::move(bar_node),
    token_comma() });
  initializers.emplace_back(designated_initializers_node::initializer{
    token_dot(), baz_token, token_equal(), std::move(qux_node) });

  auto expected_ast = std::make_unique<designated_initializers_node>(
    token_open_brace(), std::move(initializers), token_close_brace());

  const auto tokens = tokens_container_t{
    // clang-format off
    token_open_brace(),

    token_dot(),
    foo_token,
    token_equal(),
    bar_token,
    token_comma(),

    token_dot(),
    baz_token,
    token_equal(),
    qux_token,

    token_close_brace()
    // clang-format on
  };

  auto parser =
    parser_t{ dummy_err_observer, cmsl::source_view{ "" }, tokens };
  auto result_ast = parser.parse_expr();

  ASSERT_THAT(result_ast, NotNull());
  EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
}

}
