#pragma once

#include "exec/function_caller.hpp"

namespace cmsl
{
    namespace ast
    {
        class ast_context;
    }

    namespace exec
    {
        class execution;

        namespace builtin
        {
            class builtin_function_caller;
        }

        namespace infix
        {
            class infix_evaluation_context;

            class function_call_evaluator : public function_caller
            {
            public:
                explicit function_call_evaluator(infix_evaluation_context& ctx, execution &e, builtin::builtin_function_caller& builtin_caller);

                inst::instance *call(const ast::function &fun, const std::vector<inst::instance *> &params) override;

                inst::instance *call_member(inst::instance &class_instance,
                                            const ast::function &fun,
                                            const std::vector<inst::instance *> &params) override;

            private:
                inst::instance* get_and_store_function_result();

            private:
                infix_evaluation_context& m_ctx;
                execution &m_execution;
                builtin::builtin_function_caller& m_builtin_caller;
            };
        }
    }
}
