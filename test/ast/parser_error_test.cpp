#include "ast/ast_node.hpp"
#include "ast/block_node.hpp"
#include "ast/designated_initializers_node.hpp"
#include "ast/enum_node.hpp"
#include "ast/parser.hpp"
#include "ast/variable_declaration_node.cpp"
#include "errors/errors_observer.hpp"
#include "test/common/tokens.hpp"
#include "test/errors_observer_mock/errors_observer_mock.hpp"

#include <gmock/gmock.h>

namespace cmsl::ast::test {
using token_t = cmsl::lexer::token;
using token_type_t = cmsl::lexer::token_type;
using tokens_container_t = cmsl::lexer::token_container_t;
using parser_t = cmsl::ast::parser;

using ::testing::_;
using ::testing::IsNull;
using ::testing::Values;
using ::testing::TestWithParam;

using namespace cmsl::test::common;

struct errors_observer_and_mock
{
  ::testing::StrictMock<errors::test::errors_observer_mock> mock;
  errors::errors_observer err_observer;
};

using errs_t = errors_observer_and_mock;

namespace factor {
using Factor = TestWithParam<token_t>;

TEST_P(Factor, UnexpectedToken_ReportError)
{
  errs_t errs;
  EXPECT_CALL(errs.mock, notify_error(_));

  const auto tokens = tokens_container_t{ GetParam() };
  parser p{ errs.err_observer, cmsl::source_view{ "" }, tokens };
  auto result = p.parse_factor();

  EXPECT_THAT(result, IsNull());
}

const auto values = Values(
  token_undef(), token_dot(), token_open_brace(), token_close_brace(),
  token_open_square(), token_close_square(), token_equal(), token_equalequal(),
  token_minus(),      // Todo: handle unary operators
  token_minusminus(), // Todo: handle unary operators
  token_minusequal(),
  token_plus(),     // Todo: handle unary operators
  token_plusplus(), // Todo: handle unary operators
  token_plusequal(), token_amp(), token_ampamp(), token_ampequal(),
  token_pipe(), token_pipepipe(), token_pipeequal(), token_slash(),
  token_slashequal(), token_star(), token_starequal(), token_percent(),
  token_percentequal(),
  token_exclaim(), // Todo: handle unary operators
  token_exclaimequal(), token_xor(), token_xorequal(), token_semicolon(),
  token_comma(), token_less(), token_lessequal(), token_greater(),
  token_greaterequal(), token_kw_int(), token_kw_double(), token_kw_return(),
  token_kw_class(), token_kw_if(), token_kw_else(), token_kw_while(),
  token_kw_bool(), token_kw_string(), token_kw_list());

INSTANTIATE_TEST_CASE_P(Parser2ErrorsTest, Factor, values);
}

namespace binary_op {
using BinaryOperator = TestWithParam<tokens_container_t>;
TEST_P(BinaryOperator, MissingOperand_ReportError)
{
  errs_t errs;
  EXPECT_CALL(errs.mock, notify_error(_));

  const auto tokens = GetParam();
  parser p{ errs.err_observer, cmsl::source_view{ "" }, tokens };
  auto result = p.parse_expr();

  EXPECT_THAT(result, IsNull());
}

const auto id_token = token_identifier("foo");
const auto op_token = token_star();
const auto values = Values(
  tokens_container_t{ id_token, op_token },                      // foo *
  tokens_container_t{ op_token, id_token },                      // * foo
  tokens_container_t{ id_token, op_token, token_semicolon() },   // foo * ;
  tokens_container_t{ id_token, op_token, token_close_brace() }, // foo * }
  tokens_container_t{ id_token, op_token, token_close_paren() }, // foo * )
  tokens_container_t{ id_token, op_token, token_close_square() } // foo * ]
);
INSTANTIATE_TEST_CASE_P(Parser2ErrorsTest, BinaryOperator, values);
}

namespace function_call {
using FunctionCall = TestWithParam<tokens_container_t>;
TEST_P(FunctionCall, Malformed_ReportError)
{
  errs_t errs;
  EXPECT_CALL(errs.mock, notify_error(_));

  const auto tokens = GetParam();
  parser p{ errs.err_observer, cmsl::source_view{ "" }, tokens };
  auto result = p.parse_expr();

  EXPECT_THAT(result, IsNull());
}

const auto id_token = token_identifier("foo");
const auto param_token = token_identifier("bar");
const auto op_token = token_open_paren();
const auto cp_token = token_close_paren();
const auto comma_token = token_comma();
const auto values = Values(
  tokens_container_t{ id_token, op_token },                        // foo(
  tokens_container_t{ id_token, op_token, token_close_brace() },   // foo(}
  tokens_container_t{ id_token, op_token, token_close_square() },  // foo(]
  tokens_container_t{ id_token, op_token, comma_token, cp_token }, // foo(,)
  tokens_container_t{ id_token, op_token, param_token, comma_token,
                      cp_token }, // foo(bar,)
  tokens_container_t{ id_token, op_token, comma_token, param_token,
                      cp_token },                                 // foo(,bar)
  tokens_container_t{ id_token, op_token, comma_token, cp_token } // foo(,)
);
INSTANTIATE_TEST_CASE_P(Parser2ErrorsTest, FunctionCall, values);
}

namespace variable_declaration {
using VariableDeclaration = TestWithParam<tokens_container_t>;

TEST_P(VariableDeclaration, Malformed_ReportError)
{
  errs_t errs;
  EXPECT_CALL(errs.mock, notify_error(_));

  const auto tokens = GetParam();
  parser p{ errs.err_observer, cmsl::source_view{ "" }, tokens };
  auto result =
    p.parse_standalone_variable_declaration(/*export_kw=*/std::nullopt);

  EXPECT_THAT(result, IsNull());
}

const auto type_token = token_identifier("type");
const auto name_token = token_identifier("foo");
const auto semicolon_token = token_semicolon();
const auto eq_token = token_equal();
const auto expr_token = token_integer("1");

const auto values = Values(
  tokens_container_t{ type_token },                            // type
  tokens_container_t{ type_token, semicolon_token },           // type;
  tokens_container_t{ type_token, eq_token, semicolon_token }, // type =;
  tokens_container_t{ type_token, eq_token, expr_token,
                      semicolon_token },                  // type = 1;
  tokens_container_t{ type_token, name_token },           // type name
  tokens_container_t{ type_token, name_token, eq_token }, // type name =
  tokens_container_t{ type_token, name_token, eq_token,
                      semicolon_token }, // type name =;
  tokens_container_t{ type_token, name_token, eq_token, expr_token }
  // type name = 1
);

INSTANTIATE_TEST_CASE_P(ParserErrorTest, VariableDeclaration, values);
}

namespace while_ {
using While = TestWithParam<tokens_container_t>;

TEST_P(While, Malformed_ReportError)
{
  errs_t errs;
  EXPECT_CALL(errs.mock, notify_error(_));

  const auto tokens = GetParam();
  parser p{ errs.err_observer, cmsl::source_view{ "" }, tokens };
  auto result = p.parse_while_node();

  EXPECT_THAT(result, IsNull());
}

const auto while_token = token_kw_while();
const auto op_token = token_open_paren();
const auto cp_token = token_close_paren();
const auto ob_token = token_open_brace();
const auto cb_token = token_close_brace();
const auto expr_token = token_integer("1");

const auto values = Values(
  tokens_container_t{ while_token, ob_token, cb_token },           // while {}
  tokens_container_t{ while_token, op_token, ob_token, cb_token }, // while({}
  tokens_container_t{ while_token, cp_token, ob_token, cb_token }, // while){}
  tokens_container_t{ while_token, op_token, cp_token, ob_token,
                      cb_token }, // while(){}
  tokens_container_t{ while_token, op_token, expr_token, ob_token,
                      cb_token }, // while(1{}
  tokens_container_t{ while_token, expr_token, cp_token, ob_token,
                      cb_token }, // while 1){}
  tokens_container_t{ while_token, op_token, expr_token, cp_token,
                      ob_token }, // while(1){
  tokens_container_t{ while_token, op_token, expr_token, cp_token, cb_token }
  // while(1)}
);

INSTANTIATE_TEST_CASE_P(ParserErrorTest, While, values);
}

namespace if_else {
using IfElse = TestWithParam<tokens_container_t>;

TEST_P(IfElse, Malformed_ReportError)
{
  errs_t errs;
  EXPECT_CALL(errs.mock, notify_error(_));

  const auto tokens = GetParam();
  parser p{ errs.err_observer, cmsl::source_view{ "" }, tokens };
  auto result = p.parse_if_else_node();

  EXPECT_THAT(result, IsNull());
}

const auto if_token = token_kw_if();
const auto else_token = token_kw_else();
const auto op_token = token_open_paren();
const auto cp_token = token_close_paren();
const auto ob_token = token_open_brace();
const auto cb_token = token_close_brace();
const auto if_expr_token = token_integer("1");
const auto elseif_expr_token = token_integer("2");

const auto values = Values(
  tokens_container_t{ if_token, ob_token, cb_token },           // if {}
  tokens_container_t{ if_token, op_token, ob_token, cb_token }, // if({}
  tokens_container_t{ if_token, cp_token, ob_token, cb_token }, // if){}
  tokens_container_t{ if_token, op_token, cp_token },           // if()
  tokens_container_t{ if_token, op_token, cp_token, ob_token }, // if(){
  tokens_container_t{ if_token, op_token, cp_token, cb_token }, // if()}
  tokens_container_t{ if_token, op_token, cp_token, ob_token,
                      cb_token }, // if(){}
  tokens_container_t{ if_token, op_token, if_expr_token, cp_token, ob_token,
                      cb_token, else_token }, // if(1){}else
  tokens_container_t{ if_token, op_token, if_expr_token, cp_token, ob_token,
                      cb_token, else_token, ob_token }, // if(1){}else{
  tokens_container_t{ if_token, op_token, if_expr_token, cp_token, ob_token,
                      cb_token, else_token, cb_token }, // if(1){}else}

  tokens_container_t{ if_token, op_token, if_expr_token, cp_token, ob_token,
                      cb_token, else_token, if_token, ob_token,
                      cb_token }, // if(1){} else if {}
  tokens_container_t{ if_token, op_token, if_expr_token, cp_token, ob_token,
                      cb_token, else_token, if_token, op_token, ob_token,
                      cb_token }, // if(1){} else if({}
  tokens_container_t{ if_token, op_token, if_expr_token, cp_token, ob_token,
                      cb_token, else_token, if_token, cp_token, ob_token,
                      cb_token }, // if(1){} else if){}
  tokens_container_t{ if_token, op_token, if_expr_token, cp_token, ob_token,
                      cb_token, else_token, if_token, op_token,
                      cp_token }, // if(1){} else if()
  tokens_container_t{ if_token, op_token, if_expr_token, cp_token, ob_token,
                      cb_token, else_token, if_token, op_token, cp_token,
                      ob_token }, // if(1){} else if(){
  tokens_container_t{ if_token, op_token, if_expr_token, cp_token, ob_token,
                      cb_token, else_token, if_token, op_token, cp_token,
                      cb_token }, // if(1){} else if()}
  tokens_container_t{ if_token, op_token, if_expr_token, cp_token, ob_token,
                      cb_token, else_token, if_token, op_token, cp_token,
                      ob_token, cb_token }, // if(1){} else if(){}
  tokens_container_t{ if_token, op_token, if_expr_token, cp_token, ob_token,
                      cb_token, else_token, if_token, op_token, if_expr_token,
                      cp_token, ob_token, cb_token,
                      else_token }, // if(1){} else if(2){}else
  tokens_container_t{ if_token, op_token, if_expr_token, cp_token, ob_token,
                      cb_token, else_token, if_token, op_token, if_expr_token,
                      cp_token, ob_token, cb_token, else_token,
                      ob_token }, // if(1){} else if(2){}else{
  tokens_container_t{ if_token, op_token, if_expr_token, cp_token, ob_token,
                      cb_token, else_token, if_token, op_token, if_expr_token,
                      cp_token, ob_token, cb_token, else_token, cb_token }
  // if(1){} else if(2){}else}
);

INSTANTIATE_TEST_CASE_P(ParserErrorTest, IfElse, values);
}

namespace block {
using Block = TestWithParam<tokens_container_t>;

TEST_P(Block, Malformed_ReportError)
{
  errs_t errs;
  EXPECT_CALL(errs.mock, notify_error(_));

  const auto tokens = GetParam();
  parser p{ errs.err_observer, cmsl::source_view{ "" }, tokens };
  auto result = p.parse_block();

  EXPECT_THAT(result, IsNull());
}

const auto ob_token = token_open_brace();
const auto cb_token = token_open_brace();

const auto values = Values(
  tokens_container_t{ ob_token }, // {
  tokens_container_t{ cb_token }, // }
  tokens_container_t{ ob_token, token_integer("1"), token_integer("2"),
                      cb_token }, // Missing semicolon after expression { 1 2 }
  tokens_container_t{ ob_token, token_integer("1"), token_integer("2"),
                      cb_token }, // Missing semicolon after expression { 1 2 }
  tokens_container_t{
    ob_token, token_kw_return(), token_integer("1"),
    cb_token }, // Missing semicolon after expression { return 1 }
  tokens_container_t{ ob_token, token_kw_return(), cb_token }
  // Missing semicolon after expression { return }
);

INSTANTIATE_TEST_CASE_P(ParserErrorTest, Block, values);
}

namespace function {
using Function = TestWithParam<tokens_container_t>;

TEST_P(Function, Malformed_ReportError)
{
  errs_t errs;
  EXPECT_CALL(errs.mock, notify_error(_));

  const auto tokens = GetParam();
  parser p{ errs.err_observer, cmsl::source_view{ "" }, tokens };
  auto result = p.parse_function();

  EXPECT_THAT(result, IsNull());
}

const auto ret_type_token = token_identifier("ret");
const auto name_token = token_identifier("foo");
const auto op_token = token_open_paren();
const auto cp_token = token_close_paren();
const auto ob_token = token_open_brace();
const auto cb_token = token_close_brace();
const auto param1_type_token = token_identifier("param1_type");
const auto param1_name_token = token_identifier("param1_name");
const auto param2_type_token = token_identifier("param2_type");
const auto param2_name_token = token_identifier("param2_name");
const auto comma_token = token_comma();

const auto values = Values(
  tokens_container_t{ ret_type_token, op_token, cp_token, ob_token,
                      cb_token }, // ret () {}
  tokens_container_t{ name_token, op_token, cp_token, ob_token,
                      cb_token }, // foo(){}
  tokens_container_t{ ret_type_token, name_token, op_token, ob_token,
                      cb_token }, // ret foo( {}
  tokens_container_t{ ret_type_token, name_token, cp_token, ob_token,
                      cb_token }, // ret foo) {}
  tokens_container_t{ ret_type_token, name_token, op_token, cp_token,
                      ob_token }, // ret foo() {
  tokens_container_t{ ret_type_token, name_token, op_token, cp_token,
                      cb_token }, // ret foo() }
  tokens_container_t{ ret_type_token, name_token, op_token, param1_type_token,
                      cp_token, ob_token,
                      cb_token }, // ret foo(param1_type) {}
  tokens_container_t{ ret_type_token, name_token, op_token, param1_type_token,
                      comma_token, param2_type_token, param2_name_token,
                      cp_token, ob_token,
                      cb_token }, // ret foo(param1_type,param2_type name) {}
  tokens_container_t{ ret_type_token, name_token, op_token, param1_type_token,
                      param1_name_token, comma_token, param2_type_token,
                      cp_token, ob_token,
                      cb_token }, // ret foo(param1_type name,param2_type) {}
  tokens_container_t{ ret_type_token, name_token, op_token, param1_type_token,
                      param1_name_token, comma_token, cp_token, ob_token,
                      cb_token }, // ret foo(param1_type name,) {}
  tokens_container_t{ ret_type_token, name_token, op_token, comma_token,
                      param2_type_token, param2_name_token, cp_token, ob_token,
                      cb_token }, // ret foo(,param1_type name) {}
  tokens_container_t{ ret_type_token, name_token, op_token, comma_token,
                      cp_token, ob_token, cb_token } // ret foo(,) {}
);

INSTANTIATE_TEST_CASE_P(ParserErrorTest, Function, values);
}

namespace class_ {
using Class = TestWithParam<tokens_container_t>;

TEST_P(Class, Malformed_ReportError)
{
  errs_t errs;
  EXPECT_CALL(errs.mock, notify_error(_));

  const auto tokens = GetParam();
  parser p{ errs.err_observer, cmsl::source_view{ "" }, tokens };
  auto result = p.parse_class(/*export_kw=*/std::nullopt);

  EXPECT_THAT(result, IsNull());
}

const auto class_token = token_kw_class();
const auto name_token = token_identifier("foo");
const auto op_token = token_open_paren();
const auto cp_token = token_close_paren();
const auto ob_token = token_open_brace();
const auto cb_token = token_close_brace();
const auto semicolon_token = token_semicolon();
const auto member_type_token = token_kw_int();
const auto member_name_token = token_identifier("bar");
const auto method_type_token = token_kw_void();
const auto method_name_token = token_identifier("baz");
const auto expr_token = token_integer("1");

const auto values = Values(
  tokens_container_t{ name_token, ob_token, cb_token,
                      semicolon_token }, // foo {};
  tokens_container_t{ class_token, ob_token, cb_token,
                      semicolon_token }, // class {};
  tokens_container_t{ class_token, name_token, ob_token,
                      semicolon_token }, // class foo {;
  tokens_container_t{ class_token, name_token, cb_token,
                      semicolon_token }, // class foo };
  tokens_container_t{ class_token, name_token, ob_token,
                      cb_token }, // class foo {}
  tokens_container_t{ class_token, name_token, ob_token, expr_token,
                      semicolon_token, cb_token,
                      semicolon_token }, // class foo { 1; };
  tokens_container_t{ class_token, name_token, ob_token, member_type_token,
                      member_name_token, cb_token,
                      semicolon_token }, // class foo { int bar };
  tokens_container_t{
    class_token, name_token, ob_token, member_type_token, member_name_token,
    method_type_token, method_name_token, op_token, cp_token, ob_token,
    cb_token, cb_token,
    semicolon_token }, // class foo { int bar void baz() {} };
  tokens_container_t{ class_token, name_token, ob_token, method_type_token,
                      method_name_token, op_token, cp_token, ob_token,
                      cb_token, member_type_token, member_name_token, cb_token,
                      semicolon_token } // class foo { void baz() {} int bar };
);

INSTANTIATE_TEST_CASE_P(ParserErrorTest, Class, values);
}

namespace initializer_list {
using InitializerList = TestWithParam<tokens_container_t>;

TEST_P(InitializerList, Malformed_ReportError)
{
  errs_t errs;
  EXPECT_CALL(errs.mock, notify_error(_));

  const auto tokens = GetParam();
  parser p{ errs.err_observer, cmsl::source_view{ "" }, tokens };
  auto result = p.parse_initializer_list();

  EXPECT_THAT(result, IsNull());
}

const auto foo_token = token_identifier("foo");
const auto bar_token = token_identifier("bar");
const auto ob_token = token_open_brace();
const auto cb_token = token_close_brace();
const auto semicolon_token = token_semicolon();
const auto comma_token = token_comma();
const auto expr_token = token_integer("1");

const auto values = Values(
  tokens_container_t{ ob_token },                                   // {
  tokens_container_t{ cb_token },                                   // }
  tokens_container_t{ ob_token, foo_token },                        // { foo
  tokens_container_t{ foo_token, cb_token },                        // foo }
  tokens_container_t{ ob_token, foo_token, comma_token, cb_token }, // { foo, }
  tokens_container_t{ ob_token, comma_token, foo_token, cb_token }, // { ,foo
  tokens_container_t{ ob_token, comma_token, comma_token, cb_token } // { ,,
);

INSTANTIATE_TEST_CASE_P(ParserErrorTest, InitializerList, values);
}

namespace break_ {
using Break = TestWithParam<tokens_container_t>;

TEST_P(Break, Malformed_ReportError)
{
  errs_t errs;
  EXPECT_CALL(errs.mock, notify_error(_));

  const auto tokens = GetParam();
  parser p{ errs.err_observer, cmsl::source_view{ "" }, tokens };
  auto result = p.parse_initializer_list();

  EXPECT_THAT(result, IsNull());
}

const auto values = Values(tokens_container_t{ token_kw_break() }); // break

INSTANTIATE_TEST_CASE_P(ParserErrorTest, Break, values);
}

namespace namespace_ {
using Namespace = TestWithParam<tokens_container_t>;

TEST_P(Namespace, Malformed_ReportError)
{
  errs_t errs;
  EXPECT_CALL(errs.mock, notify_error(_));

  const auto tokens = GetParam();
  parser p{ errs.err_observer, cmsl::source_view{ "" }, tokens };
  auto result = p.parse_initializer_list();

  EXPECT_THAT(result, IsNull());
}

const auto nmsp = token_kw_namespace();
const auto ob = token_open_brace();
const auto cb = token_close_brace();
const auto foo_token = token_identifier("foo");
const auto bar_token = token_identifier("bar");
const auto cc = token_coloncolon();

const auto values = Values(
  tokens_container_t{ nmsp },                        // namespace
  tokens_container_t{ nmsp, ob },                    // namespace {
  tokens_container_t{ nmsp, cb },                    // namespace }
  tokens_container_t{ nmsp, cb },                    // namespace }
  tokens_container_t{ nmsp, ob, cb },                // namespace {}
  tokens_container_t{ nmsp, cc, ob, cb },            // namespace :: {}
  tokens_container_t{ nmsp, cc, foo_token, ob, cb }, // namespace ::foo {}
  tokens_container_t{ nmsp, foo_token, cc, ob, cb }, // namespace foo:: {}
  // namespace foo:bar {}
  tokens_container_t{ nmsp, foo_token, cc, cc, bar_token, ob, cb });

INSTANTIATE_TEST_CASE_P(ParserErrorTest, Namespace, values);
}

namespace designated_initializers {
using DesignatedInitializers = TestWithParam<tokens_container_t>;

TEST_P(DesignatedInitializers, Malformed_ReportError)
{
  errs_t errs;
  EXPECT_CALL(errs.mock, notify_error(_));

  const auto tokens = GetParam();
  parser p{ errs.err_observer, cmsl::source_view{ "" }, tokens };
  auto result = p.parse_designated_initializers();

  EXPECT_THAT(result, IsNull());
}

const auto dot = token_dot();
const auto ob = token_open_brace();
const auto cb = token_close_brace();
const auto equal = token_equal();
const auto comma = token_comma();
const auto foo_token = token_identifier("foo");
const auto bar_token = token_identifier("bar");
const auto baz_token = token_identifier("baz");
const auto qux_token = token_identifier("qux");

const auto values = Values(
  tokens_container_t{ ob, dot, cb },                          // { . }
  tokens_container_t{ ob, dot, foo_token, cb },               // { .foo }
  tokens_container_t{ ob, dot, foo_token, equal, cb },        // { .foo = }
  tokens_container_t{ ob, dot, foo_token, equal, comma, cb }, // { .foo = , }
  tokens_container_t{ ob, dot, foo_token, equal, bar_token, comma,
                      cb }, // { .foo = bar,  }
  tokens_container_t{ ob, dot, foo_token, equal, comma, dot, baz_token, equal,
                      qux_token, cb }); // { .foo = , .baz = qux }

INSTANTIATE_TEST_CASE_P(ParserErrorTest, DesignatedInitializers, values);
}

namespace enums {
using Enum = TestWithParam<tokens_container_t>;

TEST_P(Enum, Malformed_ReportError)
{
  errs_t errs;
  EXPECT_CALL(errs.mock, notify_error(_));

  const auto tokens = GetParam();
  parser p{ errs.err_observer, cmsl::source_view{ "" }, tokens };
  auto result = p.parse_enum(/*export_kw=*/std::nullopt);

  EXPECT_THAT(result, IsNull());
}

const auto ob = token_open_brace();
const auto cb = token_close_brace();
const auto sc = token_semicolon();
const auto comma = token_comma();
const auto en = token_kw_enum();
const auto foo_token = token_identifier("foo");
const auto bar_token = token_identifier("bar");
const auto baz_token = token_identifier("baz");
const auto qux_token = token_identifier("qux");

const auto values = Values(
  tokens_container_t{ en },                               // enum
  tokens_container_t{ en, ob },                           // enum {
  tokens_container_t{ en, cb },                           // enum }
  tokens_container_t{ en, ob, cb },                       // enum {}
  tokens_container_t{ en, ob, cb, sc },                   // enum {};
  tokens_container_t{ en, foo_token, ob },                // enum foo {
  tokens_container_t{ en, foo_token, cb },                // enum foo }
  tokens_container_t{ en, foo_token, ob, cb },            // enum foo {}
  tokens_container_t{ en, foo_token, ob, comma, cb, sc }, // enum foo {,};
  tokens_container_t{ en, foo_token, ob, bar_token, comma, cb,
                      sc }, // enum foo {bar,};
  tokens_container_t{ en, foo_token, ob, comma, bar_token, cb,
                      sc }, // enum foo {,bar};
  tokens_container_t{ en, foo_token, ob, bar_token, comma, baz_token, comma,
                      cb, sc }, // enum foo {bar, baz,};
  tokens_container_t{ en, foo_token, ob, bar_token, comma, baz_token,
                      comma }, // enum foo {bar, baz,
  tokens_container_t{ en, foo_token, ob, bar_token, comma, comma, baz_token,
                      cb, sc }); // enum foo {bar, ,baz};

INSTANTIATE_TEST_CASE_P(ParserErrorTest, Enum, values);
}

namespace imports {
using Import = TestWithParam<tokens_container_t>;

TEST_P(Import, Malformed_ReportError)
{
  errs_t errs;
  EXPECT_CALL(errs.mock, notify_error(_));

  const auto tokens = GetParam();
  parser p{ errs.err_observer, cmsl::source_view{ "" }, tokens };
  auto result = p.parse_enum(/*export_kw=*/std::nullopt);

  EXPECT_THAT(result, IsNull());
}

const auto imp = token_kw_import();
const auto foo_bar = token_string("\"foo/bar\"");
const auto id = token_identifier("baz");

const auto values = Values(tokens_container_t{ imp },      // include
                           tokens_container_t{ foo_bar },  // "foo/bar"
                           tokens_container_t{ imp, id }); // include baz

INSTANTIATE_TEST_CASE_P(ParserErrorTest, Import, values);
}
}
