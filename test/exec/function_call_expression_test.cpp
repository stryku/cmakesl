#include "exec/infix/function_call_expression.hpp"
#include "exec/infix/infix_expression_visitor.hpp"

#include "test/ast/mock/function_mock.hpp"
#include "test/ast/mock/ast_context_mock.hpp"
#include "test/common/tokens.hpp"
#include "test/exec/eval/ExpressionEvaluationTest.hpp"
#include "test/exec/mock/function_caller_mock.hpp"
#include "test/exec/mock/infix_expression_mock.hpp"
#include "test/exec/mock/instance_mock.hpp"

namespace cmsl
{
    namespace exec
    {
        namespace infix
        {
            namespace test
            {
                using testing::Eq;
                using testing::Return;
                using testing::_;

                using namespace cmsl::test::common;

                using FunctionCallExpression = ExpressionEvaluationTest<testing::Test>;

                MATCHER_P(FunctionMatcher, function_mock_ptr, "")
                {
                    return &arg == function_mock_ptr;
                };

                TEST_F(FunctionCallExpression, CorrectParametersPassAndReturn)
                {
                    auto caller_mock = std::make_unique<exec::test::function_caller_mock>();
                    auto param_1_expr_mock = std::make_unique<infix_expression_mock>();
                    auto param_2_expr_mock = std::make_unique<infix_expression_mock>();
                    auto param_1_inst_mock = std::make_unique<inst::test::instance_mock>();
                    auto param_2_inst_mock = std::make_unique<inst::test::instance_mock>();
                    auto expected_result = std::make_unique<inst::test::instance_mock>();

                    auto function_mock = std::make_unique<ast::test::function_mock>();
                    auto ast_ctx_mock = std::make_unique<ast::test::ast_context_mock>();

                    EXPECT_CALL(*ast_ctx_mock, find_function(_))
                            .WillOnce(Return(function_mock.get()));

                    m_ctx_provider.set_ast_ctx(std::move(ast_ctx_mock));

                    auto expected_params = std::vector<inst::instance *>
                            {
                                    param_1_inst_mock.get(),
                                    param_2_inst_mock.get()
                            };

                    EXPECT_CALL(*param_1_expr_mock, evaluate(_))
                            .WillOnce(Return(param_1_inst_mock.get()));

                    EXPECT_CALL(*param_2_expr_mock, evaluate(_))
                            .WillOnce(Return(param_2_inst_mock.get()));

                    auto function_mock_ptr = function_mock.get();
                    EXPECT_CALL(*caller_mock, call(FunctionMatcher(function_mock_ptr), Eq(expected_params)))
                            .WillOnce(Return(expected_result.get()));

                    using expr_t = cmsl::exec::infix::function_call_expression;
                    expr_t::params_t params;

                    params.emplace_back(std::move(param_1_expr_mock));
                    params.emplace_back(std::move(param_2_expr_mock));

                    auto expr = std::make_unique<expr_t>(*caller_mock,
                                                         token_identifier(""),
                                                         std::move(params));

                    auto eval_resutl = expr->evaluate(m_eval_ctx);

                    EXPECT_THAT(eval_resutl, Eq(expected_result.get()));
                }
            }
        }
    }
}
