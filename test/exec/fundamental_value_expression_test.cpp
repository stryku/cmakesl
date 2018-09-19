#include "test/exec/eval/ExpressionEvaluationTest.hpp"

#include "exec/infix/fundamental_value_expression.hpp"

#include "test/common/tokens.hpp"

namespace cmsl
{
    namespace test
    {
        namespace exec
        {
            namespace infix
            {
                using namespace cmsl::test::common;

                using testing::Eq;
                using testing::DoubleEq;

                namespace fundamental_value_expression
                {
                    using FundamentalValueExpression = ExpressionEvaluationTest<testing::Test>;

                    TEST_F(FundamentalValueExpression, Evaluate_BoolToken_GetBool)
                    {
                        using expr_t = cmsl::exec::infix::bool_value_expression;

                        {
                            auto expr = std::make_unique<expr_t>(token_kw_true());
                            auto result = expr->evaluate(m_eval_ctx);
                            const auto val = result->get_value();
                            const auto int_result = boost::get<bool>(val);

                            ASSERT_THAT(int_result, Eq(true));
                        }

                        {
                            auto expr = std::make_unique<expr_t>(token_kw_false());
                            auto result = expr->evaluate(m_eval_ctx);
                            const auto val = result->get_value();
                            const auto int_result = boost::get<bool>(val);

                            ASSERT_THAT(int_result, Eq(false));
                        }
                    }

                    TEST_F(FundamentalValueExpression, Evaluate_IntToken_GetInt)
                    {
                        using expr_t = cmsl::exec::infix::int_value_expression;
                        auto expr = std::make_unique<expr_t>(token_integer("123"));
                        auto result = expr->evaluate(m_eval_ctx);
                        const auto val = result->get_value();
                        const auto int_result = boost::get<cmsl::exec::inst::int_t>(val);

                        ASSERT_THAT(int_result, Eq(cmsl::exec::inst::int_t{ 123 }));
                    }

                    TEST_F(FundamentalValueExpression, Evaluate_DoubleToken_GetDouble)
                    {
                        using expr_t = cmsl::exec::infix::double_value_expression;
                        auto expr = std::make_unique<expr_t>(token_double("123.456"));
                        auto result = expr->evaluate(m_eval_ctx);
                        const auto val = result->get_value();
                        const auto double_result = boost::get<double>(val);

                        ASSERT_THAT(double_result, DoubleEq(123.456));
                    }

                    TEST_F(FundamentalValueExpression, Evaluate_StringToken_GetString)
                    {
                        using expr_t = cmsl::exec::infix::string_value_expression;
                        auto expr = std::make_unique<expr_t>(token_string("\"str\""));
                        auto result = expr->evaluate(m_eval_ctx);
                        const auto val = result->get_value();
                        const auto double_result = boost::get<std::string>(val);

                        ASSERT_THAT(double_result, Eq("str"));
                    }
                }
            }
        }
    }
}
