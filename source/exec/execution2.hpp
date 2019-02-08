#pragma once

#include "sema/sema_nodes.hpp"

#include "exec/identifiers_context.hpp"
#include "exec/instance/instance.hpp"
#include "exec/expression_evaluation_context.hpp"
#include "exec/function_caller.hpp"
#include "exec/instance/instances_holder.hpp"
#include "exec/expression_evaluation_visitor.hpp"

namespace cmsl
{
    namespace exec
    {
        class execution2 : public identifiers_context, public function_caller2
        {
        public:
            void call(const sema::function_node& fun, std::vector<inst::instance*> params) override
            {
                //auto &body = fun.body();
            }

        private:
            void execute_block(const sema::block_node& block)
            {
                for(const auto& expr : block.nodes())
                {
                    execute_node(*expr);
                    if (returning_from_function())
                    {
                        return;
                    }
                }
            }

            void execute_node(const sema::sema_node& node)
            {
                if(auto ret_node = dynamic_cast<const sema::return_node*>(&node))
                {
                    inst::instances_holder instances;
                    expression_evaluation_context ctx{ *this, instances, *this };
                    expression_evaluation_visitor visitor{ ctx };
                    ret_node->expression().visit(visitor);
                    m_function_return_value = instances.gather_ownership(visitor.result);
                }
            }

            bool returning_from_function() const
            {
                return static_cast<bool>(m_function_return_value);
            }

            expression_evaluation_context create_evaluation_context()
            {

            }

        private:
            struct callstack_frame
            {
                const sema::function_node& fun;
                execution_context exec_ctx;
            };

            std::unique_ptr<inst::instance> m_function_return_value;
            std::stack<callstack_frame> m_callstack;
        };
    }
}
