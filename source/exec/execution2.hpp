#pragma once

#include "sema/sema_nodes.hpp"

#include "exec/instance/instance.hpp"

namespace cmsl
{
    namespace exec
    {
        class execution2
        {
        public:
            void function_call(const sema::function_node& fun, std::vector<inst::instance*> params)
            {
                auto &body = fun.body();
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

                }
            }

            bool returning_from_function() const
            {
                return static_cast<bool>(m_function_return_value);
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
