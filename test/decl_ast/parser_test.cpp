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

  void visit(const component_declaration_node& node) override
  {
    m_result << "component declaration{"
             << "name:" << node.name().str() << ";derived:"
             << (node.derived_type_name() ? node.derived_type_name()->str()
                                          : cmsl::string_view{})
             << ";nodes:";
    for (const auto& n : node.nodes()) {
      n->visit(*this);
    }
    m_result << "}";
  }

  void visit(const translation_unit_node& node) override
  {
    m_result << "translation_unit_node{nodes:";
    for (const auto& n : node.nodes()) {
      n->visit(*this);
    }
    m_result << "}";
  }

  void visit(const property_access_node& node) override
  {
    m_result << "property_access{";

    for (const auto& name : node.property_access()) {
      m_result << name.str() << ';';
    }

    m_result << "}";
  }

  void visit(const property_node& node) override
  {
    m_result << "property{access:";

    node.property_access().visit(*this);

    m_result << ";assignment:" << node.assignment().str() << ";value:";

    node.value().visit(*this);

    m_result << "}";
  }

  void visit(const property_append_node& node) override
  {
    m_result << "property append{access:";

    node.property_access().visit(*this);

    m_result << ";assignment:" << node.assignment().str() << ";value:";

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

  void visit(const cmake_variable_access_node& node) override
  {
    m_result << "cmake_variable_access{variable name:"
             << node.variable_name().str() << ";as:" << node.as_type().str()
             << "}";
  }

  void visit(const import_node& node) override
  {
    m_result << "import{file name:" << node.file_name().str() << "}";
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
const auto name = token_identifier("Foo");

const auto values = Values(tokens_container_t{ ob, cb },   // {}
                           tokens_container_t{ name, ob }, // Foo {
                           tokens_container_t{ name, cb }  // Foo }
);

INSTANTIATE_TEST_CASE_P(ParserErrorTest, Component, values);
}

TEST_F(ParserTest, ComponentDeclaration_Empty_GetComponentWithoutNodes)
{
  StrictMock<cmsl::test::strings_container_mock> strings;
  errs_t errs;

  const auto component_token = token_identifier("component");
  const auto name_token = token_identifier("Component");

  auto expected_ast = std::make_unique<component_declaration_node>(
    component_token, name_token, /*colon=*/std::nullopt,
    /*derived_type_name=*/std::nullopt, token_open_brace(),
    component_declaration_node::nodes_t{}, token_close_brace());

  // clang-format off
  const auto tokens =
    tokens_container_t{ component_token,
                        name_token,
                        token_open_brace(),
                        token_close_brace() };
  // clang-format on

  auto parser =
    parser_t{ errs.observer, strings, cmsl::source_view{ "" }, tokens };
  auto result_ast = parser.parse_component_declaration();

  ASSERT_THAT(result_ast, NotNull());
  EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
}

TEST_F(ParserTest, ComponentDeclaration_DerivedEmpty_GetComponentWithoutNodes)
{
  StrictMock<cmsl::test::strings_container_mock> strings;
  errs_t errs;

  const auto component_token = token_identifier("component");
  const auto name_token = token_identifier("Component");
  const auto derived_type_name = token_identifier("base_type");

  auto expected_ast = std::make_unique<component_declaration_node>(
    component_token, name_token, token_equal(), derived_type_name,
    token_open_brace(), component_declaration_node::nodes_t{},
    token_close_brace());

  // clang-format off
  const auto tokens =
    tokens_container_t{ component_token,
                        name_token,
                        token_colon(),
                        derived_type_name,
                        token_open_brace(),
                        token_close_brace() };
  // clang-format on

  auto parser =
    parser_t{ errs.observer, strings, cmsl::source_view{ "" }, tokens };
  auto result_ast = parser.parse_component_declaration();

  ASSERT_THAT(result_ast, NotNull());
  EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
}

TEST_F(ParserTest, Component_Empty_GetComponentWithoutNodes)
{
  StrictMock<cmsl::test::strings_container_mock> strings;
  errs_t errs;

  const auto name_token = token_identifier("Component");

  auto expected_ast = std::make_unique<component_node>(
    name_token, /*derived_type_name=*/std::nullopt, token_open_brace(),
    component_node::nodes_t{}, token_close_brace());

  // clang-format off
  const auto tokens =
    tokens_container_t{ name_token,
                        token_open_brace(),
                        token_close_brace() };
  // clang-format on
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

  // clang-format off
  const auto tokens =
    tokens_container_t{ name_token,
                        token_colon(),
                        derived_type_name,
                        token_open_brace(),
                        token_close_brace() };
  // clang-format on

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
  auto property_access =
    std::make_unique<property_access_node>(std::vector{ property_name_token });
  auto value_node = std::make_unique<bool_value_node>(property_value_token);
  auto property = std::make_unique<property_node>(
    std::move(property_access), token_equal(), std::move(value_node));

  component_node::nodes_t nodes;
  nodes.emplace_back(std::move(inner_component));
  nodes.emplace_back(std::move(property));

  auto expected_ast = std::make_unique<component_node>(
    name_token, /*derived_type_name=*/std::nullopt, token_open_brace(),
    std::move(nodes), token_close_brace());

  // clang-format off
  const auto tokens = tokens_container_t{ name_token,
                                          token_open_brace(),

                                          inner_component_name_token,
                                          token_open_brace(),
                                          token_close_brace(),

                                          property_name_token,
                                          token_equal(),
                                          property_value_token,

                                          token_close_brace() };
  // clang-format on

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
  auto name_access =
    std::make_unique<property_access_node>(std::vector{ name_token });

  auto value_node = std::make_unique<bool_value_node>(value_token);
  auto expected_ast = std::make_unique<property_node>(
    std::move(name_access), token_equal(), std::move(value_node));

  const auto tokens =
    tokens_container_t{ name_token, token_equal(), value_token };
  auto parser =
    parser_t{ errs.observer, strings, cmsl::source_view{ "" }, tokens };
  auto result_ast = parser.parse_property();

  ASSERT_THAT(result_ast, NotNull());
  EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
}

TEST_F(ParserTest, PropertyAppend_WithFundamentalValue_GetPropertyNode)
{
  StrictMock<cmsl::test::strings_container_mock> strings;
  errs_t errs;

  const auto value_token = token_kw_true();
  const auto name_token = token_identifier("foo");
  auto name_access =
    std::make_unique<property_access_node>(std::vector{ name_token });

  auto value_node = std::make_unique<bool_value_node>(value_token);
  auto expected_ast = std::make_unique<property_append_node>(
    std::move(name_access), token_plusequal(), std::move(value_node));

  const auto tokens =
    tokens_container_t{ name_token, token_plusequal(), value_token };
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

const auto eq = token_equal();
const auto plus_eq = token_plusequal();
const auto name = token_identifier();
const auto value = token_kw_true();

const auto values = Values(tokens_container_t{ name, eq },       // foo =
                           tokens_container_t{ eq, value },      // = true
                           tokens_container_t{ name, value },    // foo true
                           tokens_container_t{ name, plus_eq },  // foo +=
                           tokens_container_t{ plus_eq, value }, // += true
                           tokens_container_t{ name, value }     // foo true
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

namespace cmake_variables_correct_as_type {
using CMakeVariableAccess = TestWithParam<token_t>;

TEST_P(CMakeVariableAccess, CorrectAsTypeUsed)
{
  errs_t errs;
  StrictMock<cmsl::test::strings_container_mock> strings;

  const auto cmake_variables_token = token_identifier("cmake_variables");
  const auto variable_name_token = token_identifier("FOO");
  const auto as_type_token = GetParam();

  auto expected_ast = std::make_unique<cmake_variable_access_node>(
    cmake_variables_token, variable_name_token, as_type_token);

  const auto tokens =
    tokens_container_t{ cmake_variables_token, token_dot(),
                        variable_name_token, token_dot(), as_type_token };
  parser p{ errs.observer, strings, cmsl::source_view{ "" }, tokens };
  auto result_ast = p.parse_cmake_variable_access();

  EXPECT_THAT(result_ast, NotNull());
  EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
}

const auto values =
  Values(token_identifier("as_bool"), token_identifier("as_int"),
         token_identifier("as_double"), token_identifier("as_string"),
         token_identifier("as_list"));

INSTANTIATE_TEST_CASE_P(ParserTest, CMakeVariableAccess, values);
}

namespace cmake_variables_error {
using CMakeVariableAccessMalformed = TestWithParam<tokens_container_t>;

TEST_P(CMakeVariableAccessMalformed, ReportError)
{
  errs_t errs;
  StrictMock<cmsl::test::strings_container_mock> strings;

  EXPECT_CALL(errs.mock, notify_error(_));

  const auto tokens = tokens_container_t{ GetParam() };
  parser p{ errs.observer, strings, cmsl::source_view{ "" }, tokens };
  auto result = p.parse_component();

  EXPECT_THAT(result, IsNull());
}

const auto cmake_vars = token_identifier("cmake_variables");
const auto variable_name = token_identifier("FOO");
const auto correct_as_type = token_identifier("as_bool");
const auto wrong_as_type = token_identifier("as_WRONG_TYPE");
const auto dot = token_dot();

const auto values = Values(
  tokens_container_t{ cmake_vars },                     // cmake_variables
  tokens_container_t{ cmake_vars, dot },                // cmake_variables.
  tokens_container_t{ cmake_vars, dot, variable_name }, // cmake_variables.FOO
  tokens_container_t{ cmake_vars, variable_name },      // cmake_variables FOO
  tokens_container_t{ cmake_vars, dot, variable_name,
                      dot },                  // cmake_variables.FOO.
  tokens_container_t{ cmake_vars, dot, dot }, // cmake_variables..
  tokens_container_t{ cmake_vars, dot, variable_name,
                      correct_as_type }, // cmake_variables.FOO.as_bool
  tokens_container_t{ cmake_vars, dot, variable_name, dot, wrong_as_type }
  // cmake_variables.FOO.as_WRONG_TYPE
);

INSTANTIATE_TEST_CASE_P(ParserTest, CMakeVariableAccessMalformed, values);
}

TEST_F(ParserTest, Import_GetImportNode)
{
  StrictMock<cmsl::test::strings_container_mock> strings;
  errs_t errs;

  const auto file_name_token = token_string("file_name.dcmsl");

  auto expected_ast = std::make_unique<import_node>(
    token_kw_import(), file_name_token, token_semicolon());

  // clang-format off
  const auto tokens =
    tokens_container_t{ token_kw_import(),
                        file_name_token,
                        token_semicolon() };
  // clang-format on

  auto parser =
    parser_t{ errs.observer, strings, cmsl::source_view{ "" }, tokens };
  auto result_ast = parser.parse_import();

  ASSERT_THAT(result_ast, NotNull());
  EXPECT_THAT(result_ast.get(), AstEq(expected_ast.get()));
}

namespace import_error {
using ImportMalformed = TestWithParam<tokens_container_t>;

TEST_P(ImportMalformed, ReportError)
{
  errs_t errs;
  StrictMock<cmsl::test::strings_container_mock> strings;

  EXPECT_CALL(errs.mock, notify_error(_));

  const auto tokens = tokens_container_t{ GetParam() };
  parser p{ errs.observer, strings, cmsl::source_view{ "" }, tokens };
  auto result = p.parse_component();

  EXPECT_THAT(result, IsNull());
}

const auto im = token_kw_import();
const auto file_name = token_string("file_name.dcmsl");
const auto semi = token_semicolon();

const auto values =
  Values(tokens_container_t{ im },             // import
         tokens_container_t{ im, file_name },  // import "file_name.dcmsl"
         tokens_container_t{ im, semi },       // import ;
         tokens_container_t{ file_name, semi } //  "file_name.dcmsl";
  );

INSTANTIATE_TEST_CASE_P(ParserTest, ImportMalformed, values);
}

}