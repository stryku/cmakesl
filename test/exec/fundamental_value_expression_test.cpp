#include <gmock/gmock.h>

#include "ast/builtin_ast_context.hpp"
#include "exec/infix/fundamental_value_expression.hpp"
#include "exec/infix/infix_evaluation_context.hpp"
#include "exec/execution_context.hpp"
#include "exec/instance/instances_holder.hpp"
#include "exec/context_provider.hpp"
#include "exec/instance/instance.hpp"

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
                    class context_provider : public cmsl::exec::context_provider
                    {
                    public:
                        const ast::ast_context& get_ast_ctx() const
                        {
                            return m_ast_ctx;
                        }

                        cmsl::exec::execution_context& get_exec_ctx()
                        {
                            return m_exec_ctx;
                        }

                    private:
                        cmsl::ast::builtin_ast_context m_ast_ctx;
                        cmsl::exec::execution_context m_exec_ctx;
                    };

                    class FundamentalValueExpression : public testing::Test
                    {
                    protected:
                        context_provider m_ctx_provider;
                        cmsl::exec::inst::instances_holder m_instances{ m_ctx_provider };
                        cmsl::exec::infix::infix_evaluation_context m_eval_ctx{
                            m_ctx_provider,
                            m_instances
                        };
                    };

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
