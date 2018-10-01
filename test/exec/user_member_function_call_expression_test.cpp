#include "exec/infix/user_member_function_call_expression.hpp"
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
    namespace test
    {
        namespace exec
        {
            namespace infix
            {
                using testing::Eq;
                using testing::Return;
                using testing::_;

                using namespace cmsl::test::common;

                using MemberFunctionCallExpression = ExpressionEvaluationTest<testing::Test>;

                MATCHER_P(RefByPtrMatcher, function_mock_ptr, "")
                {
                    return &arg == function_mock_ptr;
                };

                TEST_F(MemberFunctionCallExpression, CorrectParametersPassAndReturn)
                {
                    auto caller_mock = std::make_unique<function_caller_mock>();
                    auto param_1_expr_mock = std::make_unique<infix_expression_mock>();
                    auto param_2_expr_mock = std::make_unique<infix_expression_mock>();
                    auto param_1_inst_mock = std::make_unique<instance_mock>();
                    auto param_2_inst_mock = std::make_unique<instance_mock>();
                    auto expected_result = std::make_unique<instance_mock>();

                    auto class_isntance_expr_mock = std::make_unique<infix_expression_mock>();
                    auto class_instance_mock = std::make_unique<instance_mock>();

                    auto function_mock = std::make_unique<ast::function_mock>();

                    EXPECT_CALL(*class_instance_mock, get_function(_))
                                .WillOnce(Return(function_mock.get()));

                    auto expected_params = std::vector<cmsl::exec::inst::instance*>
                            {
                                    param_1_inst_mock.get(),
                                    param_2_inst_mock.get()
                            };

                    EXPECT_CALL(*class_isntance_expr_mock, evaluate(_))
                            .WillOnce(Return(class_instance_mock.get()));

                    EXPECT_CALL(*param_1_expr_mock, evaluate(_))
                            .WillOnce(Return(param_1_inst_mock.get()));

                    EXPECT_CALL(*param_2_expr_mock, evaluate(_))
                            .WillOnce(Return(param_2_inst_mock.get()));

                    auto function_mock_ptr = function_mock.get();
                    auto class_instance_ptr = class_instance_mock.get();
                    EXPECT_CALL(*caller_mock, call_member(RefByPtrMatcher(class_instance_ptr), RefByPtrMatcher(function_mock_ptr), Eq(expected_params)))
                            .WillOnce(Return(expected_result.get()));

                    using expr_t = cmsl::exec::infix::user_member_function_call_expression;
                    expr_t::params_t params;

                    params.emplace_back(std::move(param_1_expr_mock));
                    params.emplace_back(std::move(param_2_expr_mock));

                    auto expr = std::make_unique<expr_t>(*caller_mock,
                                                         std::move(class_isntance_expr_mock),
                                                         token_identifier(""),
                                                         std::move(params));

                    auto eval_resutl = expr->evaluate(m_eval_ctx);

                    EXPECT_THAT(eval_resutl, Eq(expected_result.get()));
                }
            }
        }
    }
}
