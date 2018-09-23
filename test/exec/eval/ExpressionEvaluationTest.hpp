#include <gmock/gmock.h>

#include "ast/builtin_ast_context.hpp"
#include "exec/infix/infix_evaluation_context.hpp"
#include "exec/execution_context.hpp"
#include "exec/instance/instances_holder.hpp"
#include "test/exec/test_context_provider.hpp"

namespace cmsl
{
    namespace test
    {
        namespace exec
        {
            namespace infix
            {
                class context_provider : public cmsl::exec::context_provider
                {
                public:
                    const cmsl::ast::ast_context& get_ast_ctx() const
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

                template <typename T>
                class ExpressionEvaluationTest : public T
                {
                protected:
                    using instance_t = cmsl::exec::inst::instance;
                    using int_t = cmsl::exec::inst::int_t;

                    template <typename Result>
                    Result result_val(instance_t* result)
                    {
                        auto result_val = result->get_value();
                        return boost::get<Result>(result_val);
                    }

                    int_t result_int_val(instance_t* result)
                    {
                        return result_val<int_t>(result);
                    }

                    bool result_bool_val(instance_t* result)
                    {
                        return result_val<bool>(result);
                    }

                    test_context_provider m_ctx_provider;
                    cmsl::exec::inst::instances_holder m_instances{ m_ctx_provider };
                    cmsl::exec::infix::infix_evaluation_context m_eval_ctx{
                            m_ctx_provider,
                            m_instances
                    };

                };
            }
        }
    }
}
