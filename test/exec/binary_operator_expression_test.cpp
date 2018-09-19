#include "test/exec/eval/ExpressionEvaluationTest.hpp"

#include "exec/infix/fundamental_value_expression.hpp"
#include "exec/infix/binary_operator_expression.cpp"

#include "test/common/tokens.hpp"

namespace cmsl
{
    namespace test
    {
        namespace exec
        {
            namespace infix
            {
                namespace binary_operator_expression
                {
                    using testing::Eq;
                    using testing::DoubleEq;
                    using testing::NotNull;

                    using namespace cmsl::test::common;

                    using bin_op_expr_t = cmsl::exec::infix::binary_operator_expression;
                    using int_t = cmsl::exec::inst::int_t;
                    using instance_t = cmsl::exec::inst::instance;


                    template <typename State>
                    class StatedBinaryOperatorExpression : public ExpressionEvaluationTest<testing::TestWithParam<State>>
                    {
                    protected:
                        auto make_bin_op_expr(cmsl::string_view lhs_str, token_t op, cmsl::string_view rhs_str)
                        {
                            using expr_t = cmsl::exec::infix::int_value_expression;
                            auto lhs = std::make_unique<expr_t>(token_integer(lhs_str));
                            auto rhs = std::make_unique<expr_t>(token_integer(rhs_str));
                            return std::make_unique<bin_op_expr_t>(std::move(lhs), op, std::move(rhs));
                        }

                        template <typename T>
                        T result_val(instance_t* result)
                        {
                            auto result_val = result->get_value();
                            return boost::get<T>(result_val);
                        }

                        int_t result_int_val(instance_t* result)
                        {
                            return result_val<int_t>(result);
                        }

                        bool result_bool_val(instance_t* result)
                        {
                            return result_val<bool>(result);
                        }
                    };

                    namespace integer_based
                    {
                        struct IntegerBasedBinaryOperatorExpressionTestState
                        {
                            cmsl::string_view lhs;
                            token_t op;
                            cmsl::string_view rhs;
                            int_t expected_result;
                        };

                        using State = IntegerBasedBinaryOperatorExpressionTestState;
                        using IntegerBasedBinaryOperatorExpression = StatedBinaryOperatorExpression<State>;

                        const auto values = testing::Values(
                                State{ "10", token_plus(),    "5", 15},
                                State{ "10", token_minus(),   "5", 5},
                                State{ "10", token_star(),    "5", 50},
                                State{ "10", token_slash(),   "5", 2},
                                State{ "10", token_amp(),     "2", 2},
                                State{ "8",  token_pipe(),    "2", 10},
                                State{ "8",  token_xor(),     "2", 10},
                                State{ "13", token_percent(), "5", 3}
                        );

                        TEST_P(IntegerBasedBinaryOperatorExpression, GetCorrectValue)
                        {
                            const auto& state = GetParam();
                            auto bin_op = make_bin_op_expr(state.lhs, state.op, state.rhs);
                            auto result = bin_op->evaluate(m_eval_ctx);
                            EXPECT_THAT(result, NotNull());
                            EXPECT_THAT(result_int_val(result), Eq(state.expected_result));
                        }

                        INSTANTIATE_TEST_CASE_P(InfixEvaluation, IntegerBasedBinaryOperatorExpression, values);
                    }

                    namespace relation
                    {
                        struct RelationBinaryOperatorExpressionTestState
                        {
                            cmsl::string_view val1;
                            cmsl::string_view val2;
                            token_t op;

                            struct
                            {
                                bool val1_val2;
                                bool val1_val1;
                                bool val2_val1;
                            } expected;
                        };

                        using State = RelationBinaryOperatorExpressionTestState;
                        using RelationBinaryOperatorExpression = StatedBinaryOperatorExpression<State>;

                        const auto values = testing::Values(
                                State{ "9", "5", token_less(),
                                       {
                                               false, // 9 < 5
                                               false, // 9 < 9
                                               true   // 5 < 9
                                       }},
                                State{ "9", "5", token_lessequal(),
                                       {
                                               false, // 9 <= 5
                                               true,  // 9 <= 9
                                               true   // 5 <= 9
                                       }},
                                State{ "9", "5", token_greater(),
                                       {
                                               true,  // 9 > 5
                                               false, // 9 > 9
                                               false  // 5 > 9
                                       }},
                                State{ "9", "5", token_greaterequal(),
                                       {
                                               true, // 9 >= 5
                                               true, // 9 >= 9
                                               false // 5 >= 9
                                       }}
                        );

                        TEST_P(RelationBinaryOperatorExpression, GetCorrectRelationResult)
                        {
                            const auto& state = GetParam();

                            {
                                auto bin_op = make_bin_op_expr(state.val1, state.op, state.val2);
                                auto result = bin_op->evaluate(m_eval_ctx);
                                EXPECT_THAT(result, NotNull());
                                EXPECT_THAT(result_bool_val(result), Eq(state.expected.val1_val2));
                            }
                            {
                                auto bin_op = make_bin_op_expr(state.val1, state.op, state.val1);
                                auto result = bin_op->evaluate(m_eval_ctx);
                                EXPECT_THAT(result, NotNull());
                                EXPECT_THAT(result_bool_val(result), Eq(state.expected.val1_val1));
                            }
                            {
                                auto bin_op = make_bin_op_expr(state.val2, state.op, state.val1);
                                auto result = bin_op->evaluate(m_eval_ctx);
                                EXPECT_THAT(result, NotNull());
                                EXPECT_THAT(result_bool_val(result), Eq(state.expected.val2_val1));
                            }
                        }

                        INSTANTIATE_TEST_CASE_P(InfixEvaluation, RelationBinaryOperatorExpression, values);
                    }

                    namespace comparison
                    {
                        struct ComparisonBinaryOperatorExpressionTestState
                        {
                            cmsl::string_view val1;
                            token_t op;
                            cmsl::string_view val2;
                            bool expected;
                        };

                        using State = ComparisonBinaryOperatorExpressionTestState;
                        using ComparisonBinaryOperatorExpression = StatedBinaryOperatorExpression<State>;

                        const auto values = testing::Values(
                                State{ "10", token_equalequal(),   "5",  false},
                                State{ "10", token_equalequal(),   "10", true},
                                State{ "10", token_exclaimequal(), "5",  true},
                                State{ "10", token_exclaimequal(), "10", false}
                       );

                        TEST_P(ComparisonBinaryOperatorExpression, GetCorrectComparisonResult)
                        {
                            const auto& state = GetParam();
                            auto bin_op = make_bin_op_expr(state.val1, state.op, state.val2);
                            auto result = bin_op->evaluate(m_eval_ctx);
                            EXPECT_THAT(result, NotNull());
                            EXPECT_THAT(result_bool_val(result), Eq(state.expected));
                        }

                        INSTANTIATE_TEST_CASE_P(InfixEvaluation, ComparisonBinaryOperatorExpression, values);
                    }
                }
            }
        }
    }
}
