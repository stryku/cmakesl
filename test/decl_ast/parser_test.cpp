#include "decl_ast/parser.hpp"

#include "decl_ast/ast_node_visitor.hpp"
#include "decl_ast/ast_nodes.hpp"
#include "errors/errors_observer.hpp"
#include "test/common/tokens.hpp"
#include "test/errors_observer_mock/errors_observer_mock.hpp"
#include "test/mock/strings_container_mock.hpp"

#include <gmock/gmock.h>
#include <sstream>

namespace cmsl::decl_ast::test {
using token_t = cmsl::lexer::token;
using token_type_t = cmsl::lexer::token_type;
using tokens_container_t = cmsl::lexer::token_container_t;
using parser_t = cmsl::decl_ast::parser;

using namespace cmsl::test::common;

const auto tmp_token = token_identifier("");

using ::testing::IsNull;
using ::testing::NotNull;
using ::testing::Ref;
using ::testing::Eq;
using ::testing::StrictMock;
using ::testing::_;
using ::testing::Return;
using ::testing::TestWithParam;
using ::testing::Values;

// Todo: to a common file
struct errors_observer_and_mock
{
  StrictMock<errors::test::errors_observer_mock> mock;
  errors::errors_observer observer;
};

using errs_t = errors_observer_and_mock;

class ast_tree_representation_visitor : public ast_node_visitor
{
public:
  std::string result() const { return m_result.str(); }

  void visit(const component_node& node) override
  {
    m_result << "component{"
             << "name:" << node.name().str() << ";derived:"
             << (node.derived_type_name() ? node.derived_type_name()->str()
                                          : cmsl::string_view{})
             << ";nodes:";
    for (const auto& n : node.nodes()) {
      n->visit(*this);
    }
    m_result << "}";
  }

  void visit(const property_node& node) override
  {
    m_result << "property{"
             << "name:" << node.name().str()
             << ";assignment:" << node.assignment().str() << ";value:";

    node.value().visit(*this);

    m_result << "}";
  }

  void visit(const bool_value_node& node) override
  {
    m_result << "bool value{" << node.token().str() << "}";
  }

  void visit(const int_value_node& node) override
  {
    m_result << "int value{" << node.token().str() << "}";
  }

  void visit(const double_value_node& node) override
  {
    m_result << "double value{" << node.token().str() << "}";
  }

  void visit(const string_value_node& node) override
  {
    m_result << "string value{" << node.token().str() << "}";
  }

  void visit(const list_node& node) override
  {
    m_result << "list{values:";

    for (const auto& n : node.nodes()) {
      n->visit(*this);
    }

    m_result << "}";
  }

private:
  std::ostringstream m_result;
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
};

namespace component_error {
using Component = TestWithParam<tokens_container_t>;

TEST_P(Component, Malformed_ReportError)
{
  errs_t errs;
  StrictMock<cmsl::test::strings_container_mock> strings;

  EXPECT_CALL(errs.mock, notify_error(_));

  const auto tokens = tokens_container_t{ GetParam() };
  parser p{ errs.observer, strings, cmsl::source_view{ "" }, tokens };
  auto result = p.parse_component();

  EXPECT_THAT(result, IsNull());
}

const auto ob = token_open_brace();
const auto cb = token_close_brace();
const auto name = token_identifier();

const auto values = Values(tokens_container_t{ ob, cb },   // {}
                           tokens_container_t{ name, ob }, // Foo {
                           tokens_container_t{ name, cb }  // Foo }
);

INSTANTIATE_TEST_CASE_P(ParserErrorTest, Component, values);
}

TEST_F(ParserTest, Component_Empty_GetComponentWithoutNodes)
{
  StrictMock<cmsl::test::strings_container_mock> strings;
  errs_t errs;

  const auto name_token = token_identifier("Component");

  auto expected_ast = std::make_unique<component_node>(
    name_token, /*derived_type_name=*/std::nullopt, token_open_brace(),
    component_node::nodes_t{}, token_close_brace());

  const auto tokens =
    tokens_container_t{ name_token, token_open_brace(), token_close_brace() };
  auto parser =
    parser_t{ errs.observer, strings, cmsl::source_view{ "" }, tokens };
  auto result_ast = parser.parse_component();

  ASSERT_THAT(result_ast, NotNull());
  EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
}

TEST_F(ParserTest, Component_DerivedEmpty_GetComponentWithoutNodes)
{
  StrictMock<cmsl::test::strings_container_mock> strings;
  errs_t errs;

  const auto name_token = token_identifier("Component");
  const auto derived_type_name = token_identifier("base_type");

  auto expected_ast = std::make_unique<component_node>(
    name_token, derived_type_name, token_open_brace(),
    component_node::nodes_t{}, token_close_brace());

  const auto tokens =
    tokens_container_t{ name_token, token_colon(), derived_type_name,
                        token_open_brace(), token_close_brace() };
  auto parser =
    parser_t{ errs.observer, strings, cmsl::source_view{ "" }, tokens };
  auto result_ast = parser.parse_component();

  ASSERT_THAT(result_ast, NotNull());
  EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
}

TEST_F(ParserTest,
       Component_WithInnerComponentAndProperty_GetComponentWithComponentNode)
{
  StrictMock<cmsl::test::strings_container_mock> strings;
  errs_t errs;

  const auto inner_component_name_token = token_identifier("InnerComponent");
  const auto name_token = token_identifier("Component");
  auto inner_component = std::make_unique<component_node>(
    inner_component_name_token, /*derived_type_name=*/std::nullopt,
    token_open_brace(), component_node::nodes_t{}, token_close_brace());

  const auto property_value_token = token_kw_true();
  const auto property_name_token = token_identifier("foo");
  auto value_node = std::make_unique<bool_value_node>(property_value_token);
  auto property = std::make_unique<property_node>(
    property_name_token, token_colon(), std::move(value_node));

  component_node::nodes_t nodes;
  nodes.emplace_back(std::move(inner_component));
  nodes.emplace_back(std::move(property));

  auto expected_ast = std::make_unique<component_node>(
    name_token, /*derived_type_name=*/std::nullopt, token_open_brace(),
    std::move(nodes), token_close_brace());

  const auto tokens = tokens_container_t{ name_token,
                                          token_open_brace(),

                                          inner_component_name_token,
                                          token_open_brace(),
                                          token_close_brace(),

                                          property_name_token,
                                          token_colon(),
                                          property_value_token,

                                          token_close_brace() };
  auto parser =
    parser_t{ errs.observer, strings, cmsl::source_view{ "" }, tokens };
  auto result_ast = parser.parse_component();

  ASSERT_THAT(result_ast, NotNull());
  EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
}

TEST_F(ParserTest, Value_True_GetBoolValueNode)
{
  StrictMock<cmsl::test::strings_container_mock> strings;
  errs_t errs;

  auto expected_ast = std::make_unique<bool_value_node>(token_kw_true());

  const auto tokens = tokens_container_t{ token_kw_true() };
  auto parser =
    parser_t{ errs.observer, strings, cmsl::source_view{ "" }, tokens };
  auto result_ast = parser.parse_value();

  ASSERT_THAT(result_ast, NotNull());
  EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
}

TEST_F(ParserTest, Value_False_GetBoolValueNode)
{
  StrictMock<cmsl::test::strings_container_mock> strings;
  errs_t errs;

  auto expected_ast = std::make_unique<bool_value_node>(token_kw_false());

  const auto tokens = tokens_container_t{ token_kw_false() };
  auto parser =
    parser_t{ errs.observer, strings, cmsl::source_view{ "" }, tokens };
  auto result_ast = parser.parse_value();

  ASSERT_THAT(result_ast, NotNull());
  EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
}

TEST_F(ParserTest, Value_Number_GetIntValueNode)
{
  StrictMock<cmsl::test::strings_container_mock> strings;
  errs_t errs;

  const auto number_token = token_integer("42");

  auto expected_ast = std::make_unique<int_value_node>(number_token);

  const auto tokens = tokens_container_t{ number_token };
  auto parser =
    parser_t{ errs.observer, strings, cmsl::source_view{ "" }, tokens };
  auto result_ast = parser.parse_value();

  ASSERT_THAT(result_ast, NotNull());
  EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
}

TEST_F(ParserTest, Value_FloatingPointNumber_GetDoubleValueNode)
{
  StrictMock<cmsl::test::strings_container_mock> strings;
  errs_t errs;

  const auto number_token = token_double("4.2");

  auto expected_ast = std::make_unique<double_value_node>(number_token);

  const auto tokens = tokens_container_t{ number_token };
  auto parser =
    parser_t{ errs.observer, strings, cmsl::source_view{ "" }, tokens };
  auto result_ast = parser.parse_value();

  ASSERT_THAT(result_ast, NotNull());
  EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
}

TEST_F(ParserTest, Value_String_GetDoubleValueNode)
{
  StrictMock<cmsl::test::strings_container_mock> strings;
  errs_t errs;

  const auto token = token_string("foo");

  auto expected_ast = std::make_unique<string_value_node>(token);

  const auto tokens = tokens_container_t{ token };
  auto parser =
    parser_t{ errs.observer, strings, cmsl::source_view{ "" }, tokens };
  auto result_ast = parser.parse_value();

  ASSERT_THAT(result_ast, NotNull());
  EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
}

TEST_F(ParserTest, Property_WithFundamentalValue_GetPropertyNode)
{
  StrictMock<cmsl::test::strings_container_mock> strings;
  errs_t errs;

  const auto value_token = token_kw_true();
  const auto name_token = token_identifier("foo");

  auto value_node = std::make_unique<bool_value_node>(value_token);
  auto expected_ast = std::make_unique<property_node>(
    name_token, token_colon(), std::move(value_node));

  const auto tokens =
    tokens_container_t{ name_token, token_colon(), value_token };
  auto parser =
    parser_t{ errs.observer, strings, cmsl::source_view{ "" }, tokens };
  auto result_ast = parser.parse_property();

  ASSERT_THAT(result_ast, NotNull());
  EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
}

namespace property_error {
using Property = TestWithParam<tokens_container_t>;

TEST_P(Property, Malformed_ReportError)
{
  errs_t errs;
  StrictMock<cmsl::test::strings_container_mock> strings;

  EXPECT_CALL(errs.mock, notify_error(_));

  const auto tokens = tokens_container_t{ GetParam() };
  parser p{ errs.observer, strings, cmsl::source_view{ "" }, tokens };
  auto result = p.parse_component();

  EXPECT_THAT(result, IsNull());
}

const auto colon = token_colon();
const auto name = token_identifier();
const auto value = token_kw_true();

const auto values = Values(tokens_container_t{ name, colon },  // foo :
                           tokens_container_t{ colon, value }, // : true
                           tokens_container_t{ name, value }   // foo true
);

INSTANTIATE_TEST_CASE_P(ParserErrorTest, Property, values);
}

TEST_F(ParserTest, List_Empty_GetListNodeWithoutNodes)
{
  StrictMock<cmsl::test::strings_container_mock> strings;
  errs_t errs;

  auto expected_ast = std::make_unique<list_node>(
    token_open_square(), list_node::nodes_t{}, token_close_square());

  const auto tokens =
    tokens_container_t{ token_open_square(), token_close_square() };
  auto parser =
    parser_t{ errs.observer, strings, cmsl::source_view{ "" }, tokens };
  auto result_ast = parser.parse_value();

  ASSERT_THAT(result_ast, NotNull());
  EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
}

TEST_F(ParserTest, List_WithNode_GetListNodeWithNode)
{
  StrictMock<cmsl::test::strings_container_mock> strings;
  errs_t errs;

  auto value_node = std::make_unique<bool_value_node>(token_kw_true());

  list_node::nodes_t nodes;
  nodes.emplace_back(std::move(value_node));

  auto expected_ast = std::make_unique<list_node>(
    token_open_square(), std::move(nodes), token_close_square());

  const auto tokens = tokens_container_t{ token_open_square(), token_kw_true(),
                                          token_close_square() };
  auto parser =
    parser_t{ errs.observer, strings, cmsl::source_view{ "" }, tokens };
  auto result_ast = parser.parse_value();

  ASSERT_THAT(result_ast, NotNull());
  EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
}

TEST_F(ParserTest, List_WithNodeAndComma_GetListNodeWithNode)
{
  StrictMock<cmsl::test::strings_container_mock> strings;
  errs_t errs;

  auto value_node = std::make_unique<bool_value_node>(token_kw_true());

  list_node::nodes_t nodes;
  nodes.emplace_back(std::move(value_node));

  auto expected_ast = std::make_unique<list_node>(
    token_open_square(), std::move(nodes), token_close_square());

  const auto tokens =
    tokens_container_t{ token_open_square(), token_kw_true(), token_comma(),
                        token_close_square() };
  auto parser =
    parser_t{ errs.observer, strings, cmsl::source_view{ "" }, tokens };
  auto result_ast = parser.parse_value();

  ASSERT_THAT(result_ast, NotNull());
  EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
}

TEST_F(ParserTest, List_WithNodes_GetListNodeWithNodes)
{
  StrictMock<cmsl::test::strings_container_mock> strings;
  errs_t errs;

  auto value_node = std::make_unique<bool_value_node>(token_kw_true());
  auto value2_node = std::make_unique<bool_value_node>(token_kw_false());

  list_node::nodes_t nodes;
  nodes.emplace_back(std::move(value_node));
  nodes.emplace_back(std::move(value2_node));

  auto expected_ast = std::make_unique<list_node>(
    token_open_square(), std::move(nodes), token_close_square());

  const auto tokens =
    tokens_container_t{ token_open_square(), token_kw_true(), token_comma(),
                        token_kw_false(), token_close_square() };
  auto parser =
    parser_t{ errs.observer, strings, cmsl::source_view{ "" }, tokens };
  auto result_ast = parser.parse_value();

  ASSERT_THAT(result_ast, NotNull());
  EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
}

namespace list_error {
using List = TestWithParam<tokens_container_t>;

TEST_P(List, Malformed_ReportError)
{
  errs_t errs;
  StrictMock<cmsl::test::strings_container_mock> strings;

  EXPECT_CALL(errs.mock, notify_error(_));

  const auto tokens = tokens_container_t{ GetParam() };
  parser p{ errs.observer, strings, cmsl::source_view{ "" }, tokens };
  auto result = p.parse_component();

  EXPECT_THAT(result, IsNull());
}

const auto os = token_open_square();
const auto cs = token_close_square();
const auto comma = token_comma();
const auto value = token_kw_true();
const auto value2 = token_kw_false();

const auto values =
  Values(tokens_container_t{ os },                   // [
         tokens_container_t{ cs },                   // ]
         tokens_container_t{ os, comma, value, cs }, // [ ,true ]
         tokens_container_t{ os, value, value2, cs } // [ true false ]
  );

INSTANTIATE_TEST_CASE_P(ParserErrorTest, List, values);
}

}