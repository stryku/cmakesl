#pragma once

#include <sema/user_sema_function.hpp>
#include "sema/sema_nodes.hpp"

#include "exec/identifiers_context.hpp"
#include "exec/instance/instance.hpp"
#include "exec/expression_evaluation_context.hpp"
#include "exec/function_caller.hpp"
#include "exec/instance/instances_holder.hpp"
#include "exec/expression_evaluation_visitor.hpp"
#include "exec/builtin_function_caller2.hpp"
#include "sema/builtin_sema_function.hpp"
#include "exec/instance/instance_factory.hpp"

namespace cmsl
{
    namespace exec
    {
        class execution2 : public identifiers_context, public function_caller2
        {
        public:
            // Todo: consider returning a reference
            std::unique_ptr<inst::instance> call(const sema::sema_function& fun,
                                 const std::vector<inst::instance*>& params,
                                 inst::instances_holder_interface& instances) override
            {
                if(auto user_function = dynamic_cast<const sema::user_sema_function*>(&fun))
                {
                    enter_function_scope(fun);
                    execute_block(user_function->body());
                    leave_function_scope();
                }
                else
                {
                    // Todo: handle builtin functions
                }

                return std::move(m_function_return_value);

            }

            // Todo: consider returning a reference
            std::unique_ptr<inst::instance> call_member(inst::instance& class_instance,
                                        const sema::sema_function& fun,
                                        const std::vector<inst::instance*>& params,
                                        inst::instances_holder_interface& instances) override
            {
                if(auto user_function = dynamic_cast<const sema::user_sema_function*>(&fun))
                {
                    enter_function_scope(fun, class_instance);
                    execute_block(user_function->body());
                    leave_function_scope();
                    return std::move(m_function_return_value);
                }
                else
                {
                    auto builtin_function = dynamic_cast<const sema::builtin_sema_function*>(&fun);
                    return builtin_function_caller2{instances}.call_member(class_instance, builtin_function->kind(), params);
                }
            }

            inst::instance* lookup_identifier(cmsl::string_view identifier) override
            {
                return m_callstack.top().exec_ctx.get_variable(identifier);
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
                    m_function_return_value = execute_infix_expression(node);
                }
                else if(auto var_decl = dynamic_cast<const sema::variable_declaration_node*>(&node))
                {
                    auto& exec_ctx = m_callstack.top().exec_ctx;
                    inst::instances_holder instances{ current_context() };
                    std::unique_ptr<inst::instance> created_instance;

                    if(auto initialization = var_decl->initialization())
                    {
                        created_instance = execute_infix_expression(*initialization);
                    }
                    else
                    {
                        // Todo: create variable without instances holder
                        auto variable_instance_ptr = instances.create2(var_decl->type());
                        created_instance = instances.gather_ownership(variable_instance_ptr);
                    }

                    exec_ctx.add_variable(var_decl->name().str(), std::move(created_instance));
                }
                else
                {
                    // A stand alone infix expression.
                    (void)execute_infix_expression(node);
                }
            }

            bool returning_from_function() const
            {
                return static_cast<bool>(m_function_return_value);
            }

            const sema::sema_context_interface& current_context() const
            {
                const auto& current_function = m_callstack.top().fun;
                return current_function.context();
            }

            void enter_function_scope(const sema::sema_function& fun)
            {
                m_callstack.push(callstack_frame{fun});
                m_callstack.top().exec_ctx.enter_scope();
            }

            void enter_function_scope(const sema::sema_function& fun, inst::instance& class_instance)
            {
                m_callstack.push(callstack_frame{fun});
                m_callstack.top().exec_ctx.enter_member_function_scope(&class_instance);
            }

            void leave_function_scope()
            {
                m_callstack.top().exec_ctx.leave_scope();
                m_callstack.pop();
            }

            std::unique_ptr<inst::instance> execute_infix_expression(const sema::sema_node& node)
            {
                inst::instances_holder instances{ current_context() };
                expression_evaluation_context ctx{ *this, instances, *this };
                expression_evaluation_visitor visitor{ ctx };
                node.visit(visitor);
                return visitor.result->copy(); // Todo: it'd be good to move instead of copy.
            }

        private:
            struct callstack_frame
            {
                const sema::sema_function& fun;
                execution_context exec_ctx;
            };

            std::unique_ptr<inst::instance> m_function_return_value;
            std::stack<callstack_frame> m_callstack;
        };
    }
}
