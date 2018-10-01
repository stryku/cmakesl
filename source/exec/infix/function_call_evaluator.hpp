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

        namespace infix
        {
            class infix_evaluation_context;

            class function_call_evaluator : public function_caller
            {
            public:
                explicit function_call_evaluator(infix_evaluation_context& ctx, execution &e);

                inst::instance *call(const ast::function &fun, const std::vector<inst::instance *> &params) override;

                inst::instance *call_member(inst::instance &class_instance,
                                            const ast::function &fun,
                                            const std::vector<inst::instance *> &params) override;

            private:
                inst::instance* get_and_store_function_result();

            private:
                infix_evaluation_context& m_ctx;
                execution &m_execution;
            };
        }
    }
}
