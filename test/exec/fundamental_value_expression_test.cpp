#include "test/exec/eval/ExpressionEvaluationTest.hpp"

#include "exec/infix/fundamental_value_expression.hpp"

#include "test/common/tokens.hpp"

namespace cmsl
{
    namespace exec
    {
        namespace infix
        {
            namespace test
            {
                using namespace cmsl::test::common;

                using testing::Eq;
                using testing::DoubleEq;
                using testing::NotNull;

                namespace fundamental_value_expression
                {
                    using FundamentalValueExpression = ExpressionEvaluationTest<testing::Test>;

                    TEST_F(FundamentalValueExpression, DISABLED_Evaluate_BoolToken_GetBool)
                    {
                        {
                            auto expr = std::make_unique<bool_value_expression>(token_kw_true());
                            auto result = expr->evaluate(m_eval_ctx);
                            const auto val = result->get_value();

                            ASSERT_THAT(result, NotNull());
                            ASSERT_THAT(result_bool_val(result), Eq(true));
                        }

                        {
                            auto expr = std::make_unique<bool_value_expression>(token_kw_false());
                            auto result = expr->evaluate(m_eval_ctx);
                            const auto val = result->get_value();

                            ASSERT_THAT(result, NotNull());
                            ASSERT_THAT(result_bool_val(result), Eq(false));
                        }
                    }

                    TEST_F(FundamentalValueExpression, DISABLED_Evaluate_IntToken_GetInt)
                    {
                        auto expr = std::make_unique<int_value_expression>(token_integer("123"));
                        auto result = expr->evaluate(m_eval_ctx);

                        ASSERT_THAT(result, NotNull());
                        ASSERT_THAT(result_int_val(result), Eq(inst::int_t{123}));
                    }

                    TEST_F(FundamentalValueExpression, DISABLED_Evaluate_DoubleToken_GetDouble)
                    {
                        auto expr = std::make_unique<double_value_expression>(token_double("123.456"));
                        auto result = expr->evaluate(m_eval_ctx);

                        ASSERT_THAT(result, NotNull());
                        ASSERT_THAT(result_double_val(result), DoubleEq(123.456));
                    }

                    TEST_F(FundamentalValueExpression, DISABLED_Evaluate_StringToken_GetString)
                    {
                        auto expr = std::make_unique<string_value_expression>(token_string("\"str\""));
                        auto result = expr->evaluate(m_eval_ctx);

                        ASSERT_THAT(result, NotNull());
                        ASSERT_THAT(result_string_val(result), Eq("str"));
                    }
                }
            }
        }
    }
}
