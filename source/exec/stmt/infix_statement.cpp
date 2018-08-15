#include "exec/stmt/infix_statement.hpp"
#include "exec/exec.hpp"
#include "ast/infix_node.hpp"
#include "exec/instance.hpp"
#include "ast/type_kind.hpp"
#include "ast/ast_context.hpp"
#include "exec/named_instance.hpp"
#include "exec/stmt/infix_to_onp.hpp"

#include "lexer/token/token.hpp"

namespace cmsl
{
    namespace exec
    {
        namespace stmt
        {
            infix_statement::infix_statement(const ast::infix_node& node, int& result)
                : m_node{ node }
                , m_result{ result }
            {}

            void infix_statement::execute(executor& e)
            {
                const auto onp_tokens = to_onp(e);
                m_result = execute_onp(onp_tokens, e);
            }

            infix_statement::tokens_container_t infix_statement::to_onp(executor& e) const
            {
                const auto& tokens = m_node.get_tokens();
                return infix_to_onp{ tokens, e.get_ast_ctx() }.convert();

                /*lexer::token::token_container_t result;
                std::stack<lexer::token::token> stack;

                const auto& tokens = m_node.get_tokens();
                for(auto token_it = std::cbegin(tokens);
                    token_it != std::cend(tokens);
                    ++token_it)
                {
                    const auto& token = *token_it;
                    const auto token_type = token.get_type();

                    if (token_type == token_type_t::integer)
                    {
                        result.emplace_back(token);
                    }
                    else if (token_type == token_type_t::identifier)
                    {
                        const auto fun = e.get_ast_ctx().find_function(token.str());
                        if (!fun)
                        {
                            result.emplace_back(token);
                        }
                        else
                        {
                            stack.push(token);

                        }
                    }
                    else if (token_type == token_type_t::plus)
                    {
                        while (!stack.empty())
                        {
                            const auto op = stack.top();
                            stack.pop();
                            result.emplace_back(op);
                        }

                        stack.push(token);
                    }
                    else if (token_type == token_type_t::open_paren)
                    {
                        stack.push(token);
                    }
                    else if (token_type == token_type_t::close_paren)
                    {
                        while (!stack.empty() && stack.top().get_type() != token_type_t::open_paren)
                        {
                            const auto op = stack.top();
                            stack.pop();
                            result.emplace_back(op);
                        }

                        stack.pop();
                    }*/
                /*}

                while (!stack.empty())
                {
                    const auto tok = stack.top();
                    stack.pop();
                    result.emplace_back(tok);
                }*/

                //return result;
            }

            infix_statement::tokens_container_t infix_statement::function_call_to_onp(executor& e, const tokens_container_t& tokens) const
            {
                tokens_container_t out;



                return out;
            }

            int infix_statement::execute_onp(const tokens_container_t& onp_tokens, executor& e)
            {
                std::stack<std::unique_ptr<instance>> stack;

                for (const auto& token : onp_tokens)
                {
                    const auto token_type = token.get_type();
                    if (token_type == token_type_t::integer)
                    {
                        const auto& type = *e.get_ast_ctx().find_type("int");
                        auto inst = std::make_unique<instance>(type);
                        const auto val = std::stoi(token.str().to_string());
                        inst->assign(val);
                        stack.push(std::move(inst));
                    }
                    else if (token_type == token_type_t::identifier)
                    {
                        const auto var = e.get_ctx().get_variable(token.str());

                        if (!var)
                        {
                            // todo raise identifier not found error
                            return 0;
                        }

                        const auto& type = *e.get_ast_ctx().find_type("int");
                        auto inst = std::make_unique<named_instance>(type, token.str(), e.get_ctx());
                        stack.push(std::move(inst));
                    }
                    else
                    {
                        const auto a = std::move(stack.top());
                        stack.pop();
                        const auto b = std::move(stack.top());
                        stack.pop();

                        switch (token_type)
                        {
                            case token_type_t::plus:
                            {
                                const auto& type = *e.get_ast_ctx().find_type("int");
                                auto inst = std::make_unique<instance>(type);
                                inst->assign(b->get_value() + a->get_value());
                                stack.push(std::move(inst));
                            }
                            break;
                            case token_type_t::minus:
                            {
                                const auto& type = *e.get_ast_ctx().find_type("int");
                                auto inst = std::make_unique<instance>(type);
                                inst->assign(b->get_value() - a->get_value());
                                stack.push(std::move(inst));
                            }
                            break;
                        }
                    }
                }

                return stack.top()->get_value();
            }
        }
    }
}
