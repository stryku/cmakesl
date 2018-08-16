#include "exec/stmt/onp_executor.hpp"
#include "exec/exec.hpp"
#include "exec/fundamental_instance.hpp"
#include "exec/named_instance.hpp"
#include "lexer/token/token.hpp"
#include "ast/ast_context.hpp"
#include "ast/function_node.hpp"
#include "exec/fundamental_instance.hpp"
#include "exec/instance_factory.hpp"

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
                        const auto val = std::stoi(token.str().to_string());
                        auto inst = get_factory().create(val);
                        m_stack.push(std::move(inst));
                    }
                    else if (token_type == token_type_t::identifier)
                    {
                        if (const auto var = m_exec.get_exec_ctx().get_variable(token.str()))
                        {
                            auto inst = get_factory().create(token.str());
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

                        auto result = apply_operator(b.get(), token_type, b.get());
                        m_stack.push(std::move(result));
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
                auto inst = get_factory().create(ret_val);
                m_stack.push(std::move(inst));
            }

            std::unique_ptr<instance> onp_executor::get_top_and_pop()
            {
                auto inst = std::move(m_stack.top());
                m_stack.pop();
                return std::move(inst);
            }

            std::unique_ptr<instance> onp_executor::apply_operator(instance* lhs, token_type_t op, instance* rhs)
            {
                auto result = lhs->get_value() + rhs->get_value();
                return get_factory().create(result);
            }

            instance_factory onp_executor::get_factory()
            {
                return instance_factory{ m_exec.get_ast_ctx(), m_exec.get_exec_ctx() };
            }
        }
    }
}
