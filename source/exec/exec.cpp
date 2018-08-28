#include "exec/exec.hpp"

#include "common/assert.hpp"
#include "ast/ast_node.hpp"
#include "lexer/lexer.hpp"
#include "errors/errors_observer.hpp"
#include "ast/ast_builder.hpp"
#include "ast/builtin_ast_context.hpp"
#include "ast/return_node.hpp"
#include "ast/declaration_node.hpp"
#include "ast/if_else_node.hpp"

#include "exec/stmt/return_statement.hpp"
#include "exec/stmt/declaration_statement.hpp"
#include "exec/stmt/if_else_statement.hpp"

#include <iostream>
#include <exec/stmt/infix_statement.hpp>

namespace cmsl
{
    namespace exec
    {
        int executor::execute(cmsl::string_view source)
        {
            errors::errors_observer err_observer;
            lexer::lexer lex{ err_observer , source };
            const auto tokens = lex.lex();

            ast::ast_builder builder;
            ast::builtin_ast_context ctx;
            auto global_ast_ctx = builder.build(ctx, err_observer, tokens);
            auto main_function = global_ast_ctx->find_function("main");
            function_call(*main_function, {});
            const auto main_result = *m_function_return_value;
            return main_result;
        }

        void executor::execute_function_call(const ast::function_node& fun, std::vector<inst::instance*> parameters)
        {
            m_function_return_value = boost::none;

            const auto& params_decl = fun.get_params_declarations();
            const auto params_count = params_decl.size();

            for (auto i = 0u; i < params_count; ++i)
            {
                get_exec_ctx().add_variable(params_decl[i].name.str(),
                                            parameters[i]->copy());
            }

            execute_block(fun.get_body());
            return_from_function();
        }


        void executor::member_function_call(const ast::function_node& fun,
                                            std::vector<inst::instance*> parameters,
                                            inst::instance* class_instance)
        {
            m_ast_context = &fun.get_ast_context();
            m_callstack.push({ &fun, execution_context{} });
            get_exec_ctx().enter_member_function_scope(class_instance);
            execute_function_call(fun, std::move(parameters));
        }

        void executor::function_call(const ast::function_node& fun, std::vector<inst::instance*> parameters)
        {
            m_ast_context = &fun.get_ast_context();
            m_callstack.push({ &fun, execution_context{} });
            get_exec_ctx().enter_scope();
            execute_function_call(fun, std::move(parameters));
        }

        bool executor::execute_function_expression(ast::ast_node& expr)
        {
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
                    int result;
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

                default:
                    CMSL_UNREACHABLE("Executing not implemented statement.");
            }

            return false;
        }

        void executor::return_from_function()
        {
            get_exec_ctx().leave_scope();
            m_callstack.pop();
        }

        execution_context& executor::get_exec_ctx()
        {
            assert(!m_callstack.empty());
            return m_callstack.top().exec_ctx;
        }

        const ast::ast_context& executor::get_ast_ctx() const
        {
            return *m_ast_context;
        }

        int executor::get_function_return_value() const
        {
            return *m_function_return_value;
        }

        void executor::block(const ast::block_node &block_node)
        {
            get_exec_ctx().enter_scope(); // todo RAII
            execute_block(block_node);
            get_exec_ctx().leave_scope();
        }

        void executor::execute_block(const ast::block_node &block_node)
        {
            const auto expressions = block_node.get_expressions();
            for (auto expr : expressions)
            {
                const auto returned_from_function = execute_function_expression(*expr);
                if (returning_from_function())
                {
                    return;
                }
            }
        }

        void executor::return_from_function(int value)
        {
            m_function_return_value = value;
        }

        bool executor::returning_from_function() const
        {
            return m_function_return_value.is_initialized();
        }
    }
}
