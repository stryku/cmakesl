#include "exec/execution.hpp"

#include "common/assert.hpp"
#include "ast/ast_node.hpp"
#include "lexer/lexer.hpp"
#include "errors/errors_observer.hpp"
#include "ast/ast_builder.hpp"
#include "ast/builtin_ast_context.hpp"
#include "ast/return_node.hpp"
#include "ast/declaration_node.hpp"
#include "ast/if_else_node.hpp"
#include "ast/while_node.hpp"

#include "ast/builtin_function.hpp"
#include "common/type_traits.hpp"
#include "exec/instance/instance_converter.hpp"
#include "exec/instance/instances_holder.hpp"

#include "exec/stmt/return_statement.hpp"
#include "exec/stmt/declaration_statement.hpp"
#include "exec/stmt/if_else_statement.hpp"
#include "exec/stmt/infix_statement.hpp"
#include "exec/stmt/while_statement.hpp"

#include <iostream>

namespace cmsl
{
    namespace exec
    {
        execution::execution(facade::cmake_facade &f)
            : m_cmake_facade{ f }
        {}
        
        void execution::execute_function_call(const ast::user_function_node& fun, std::vector<inst::instance*> parameters)
        {
            m_function_return_value.reset();

            const auto& params_decl = fun.get_params_declarations();
            const auto params_count = params_decl.size();

            for (auto i = 0u; i < params_count; ++i)
            {
                get_exec_ctx().add_variable(params_decl[i].name->str(),
                                            parameters[i]->copy());
            }

            execute_block(fun.get_body());
            return_from_function();
        }


        void execution::member_function_call(const ast::user_function_node& fun,
                                                   std::vector<inst::instance*> parameters,
                                                   inst::instance* class_instance)
        {
            m_ast_context = &fun.get_ast_context();
            m_callstack.push({ &fun, execution_context{} });
            get_exec_ctx().enter_member_function_scope(class_instance);
            execute_function_call(fun, std::move(parameters));
        }

        void execution::function_call(const ast::user_function_node& fun, std::vector<inst::instance*> parameters)
        {
            m_ast_context = &fun.get_ast_context();
            m_callstack.push({ &fun, execution_context{} });
            get_exec_ctx().enter_scope();
            execute_function_call(fun, std::move(parameters));
        }

        bool execution::execute_function_expression(ast::ast_node& expr)
        {
            // todo change type to dynamic casts
            switch (expr.get_type())
            {
                case ast::ast_node_type::ret:
                {
                    stmt::return_statement ret_stmt{ dynamic_cast<ast::return_node&>(expr) };
                    ret_stmt.execute(*this);
                    return true;
                }

                case ast::ast_node_type::declaration:
                {
                    stmt::declaration_statement decl_stmt{ dynamic_cast<ast::declaration_node&>(expr) };
                    decl_stmt.execute(*this);
                }
                    break;

                case ast::ast_node_type::infix:
                {
                    std::unique_ptr<inst::instance> result;
                    stmt::infix_statement infix_stmt{ dynamic_cast<ast::infix_node&>(expr), result };
                    infix_stmt.execute(*this);
                }
                    break;

                case ast::ast_node_type::if_else:
                {
                    stmt::if_else_statement if_else{ dynamic_cast<ast::if_else_node&>(expr) };
                    if_else.execute(*this);
                }
                    break;

                case ast::ast_node_type::while_:
                {
                    stmt::while_statement while_stmt{ dynamic_cast<ast::while_node&>(expr) };
                    while_stmt.execute(*this);
                }
                    break;

                default:
                    CMSL_UNREACHABLE("Executing not implemented statement.");
            }

            return false;
        }

        void execution::return_from_function()
        {
            get_exec_ctx().leave_scope();
            m_callstack.pop();
        }

        execution_context& execution::get_exec_ctx()
        {
            assert(!m_callstack.empty());
            return m_callstack.top().exec_ctx;
        }

        const ast::ast_context& execution::get_ast_ctx() const
        {
            return *m_ast_context;
        }

        std::unique_ptr<inst::instance> execution::get_function_return_value() const
        {
            return m_function_return_value->copy();
        }

        void execution::block(const ast::block_node &block_node)
        {
            get_exec_ctx().enter_scope(); // todo RAII
            execute_block(block_node);
            get_exec_ctx().leave_scope();
        }

        void execution::execute_block(const ast::block_node &block_node)
        {
            const auto expressions = block_node.get_expressions();
            for (auto expr : expressions)
            {
                execute_function_expression(*expr);
                if (returning_from_function())
                {
                    return;
                }
            }
        }

        void execution::return_from_function(std::unique_ptr<inst::instance> value)
        {
            const auto& fun_ret_type = get_current_function_return_type();
            inst::instances_holder instances{ *this };
            inst::instance_converter converter{ instances };
            auto inst = converter.convert_to_type(value.get(), fun_ret_type);
            m_function_return_value = instances.gather_ownership(inst);
        }

        bool execution::returning_from_function() const
        {
            return m_function_return_value.get() != nullptr;
        }

        const ast::type& execution::get_current_function_return_type() const
        {
            return m_callstack.top().fun->get_return_type();
        }
    }
}

