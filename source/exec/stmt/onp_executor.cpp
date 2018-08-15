#include "exec/stmt/onp_executor.hpp"
#include "exec/exec.hpp"
#include "exec/instance.hpp"
#include "exec/named_instance.hpp"
#include "lexer/token/token.hpp"
#include "ast/ast_context.hpp"
#include "ast/function_node.hpp"

namespace cmsl
{
    namespace exec
    {
        namespace stmt
        {
            onp_executor::onp_executor(const tokens_container_t& onp_tokens, executor& e, int& result)
                : m_tokens{ onp_tokens }
                , m_exec{ e }
                , m_result{ result }
            {}

            void onp_executor::execute()
            {
                for (const auto& token : m_tokens)
                {
                    const auto token_type = token.get_type();
                    if (token_type == token_type_t::integer)
                    {
                        const auto& type = *m_exec.get_ast_ctx().find_type("int");
                        auto inst = std::make_unique<instance>(type);
                        const auto val = std::stoi(token.str().to_string());
                        inst->assign(val);
                        m_stack.push(std::move(inst));
                    }
                    else if (token_type == token_type_t::identifier)
                    {
                        if (const auto var = m_exec.get_exec_ctx().get_variable(token.str()))
                        {
                            const auto& type = *m_exec.get_ast_ctx().find_type("int");
                            auto inst = std::make_unique<named_instance>(type, token.str(), m_exec.get_exec_ctx());
                            m_stack.push(std::move(inst));
                        }
                        else if(const auto fun = m_exec.get_ast_ctx().find_function(token.str()))
                        {
                            execute_function_call(*fun);
                        }
                        else
                        {
                            // todo raise identifier not found error
                            return;
                        }
                    }
                    else
                    {
                        const auto a = get_top_and_pop();
                        const auto b = get_top_and_pop();

                        switch (token_type)
                        {
                            case token_type_t::plus:
                            {
                                const auto& type = *m_exec.get_ast_ctx().find_type("int");
                                auto inst = std::make_unique<instance>(type);
                                inst->assign(b->get_value() + a->get_value());
                                m_stack.push(std::move(inst));
                            }
                            break;
                            case token_type_t::minus:
                            {
                                const auto& type = *m_exec.get_ast_ctx().find_type("int");
                                auto inst = std::make_unique<instance>(type);
                                inst->assign(b->get_value() - a->get_value());
                                m_stack.push(std::move(inst));
                            }
                            break;
                        }
                    }
                }

                m_result = m_stack.top()->get_value();
            }

            void onp_executor::execute_function_call(const ast::function_node& fun)
            {
                auto& exec_ctx = m_exec.get_exec_ctx();
                std::vector<std::unique_ptr<instance>> params;

                for (const auto& param_decl : fun.get_params_declarations())
                {
                    params.emplace_back(get_top_and_pop());
                }

                // Parameters are on stack in reverese order
                std::reverse(std::begin(params), std::end(params));

                m_exec.function_call(fun, std::move(params));

                const auto ret_val = m_exec.get_function_return_value();
                const auto& type = *m_exec.get_ast_ctx().find_type("int");
                auto inst = std::make_unique<instance>(type, ret_val);
                m_stack.push(std::move(inst));
            }

            std::unique_ptr<instance> onp_executor::get_top_and_pop()
            {
                auto inst = std::move(m_stack.top());
                m_stack.pop();
                return std::move(inst);
            }
        }
    }
}
