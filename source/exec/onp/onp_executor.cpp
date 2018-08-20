#include "exec/onp/onp_executor.hpp"
#include "exec/exec.hpp"
#include "exec/instance/named_instance.hpp"
#include "ast/ast_context.hpp"
#include "exec/instance/instance_factory.hpp"
#include "exec/onp/onp_entry.hpp"

namespace
{
    using instance_t = cmsl::exec::inst::instance;
    using token_t = cmsl::lexer::token::token;
    using token_type_t = cmsl::lexer::token::token_type;
    using execution_context_t = cmsl::exec::execution_context;
    using instance_factory_t = cmsl::exec::inst::instance_factory;
    using instances_holder_t = cmsl::exec::inst::instances_holder;

    class operator_visitor
    {
    public:
        explicit operator_visitor(token_type_t op,
                                  execution_context_t& exec_ctx,
                                  instances_holder_t& instances)
            : m_operator{ op }
            , m_exec_ctx{ exec_ctx }
            , m_instances{ instances }
        {}

        auto operator()(instance_t* lhs, instance_t* rhs)
        {
            // todo handle all operators
            const auto val = lhs->get_value() + rhs->get_value();
            return m_instances.create(val);
        };

        auto operator() (instance_t* lhs, const token_t& token)
        {
            // todo handle all operators
            return lhs->get_member(token.str());
        };

        auto operator()(const token_t& lhs, const token_t& rhs)
        {
            // todo handle all operators
            auto lhs_instance = m_exec_ctx.get_variable(lhs.str());
            return lhs_instance->get_member(rhs.str());
        };

        instance_t* operator()(const token_t& lhs, instance_t* rhs)
        {
            // todo handle all operators
            auto lhs_instance = m_exec_ctx.get_variable(lhs.str());
            const auto val = lhs_instance->get_value() + rhs->get_value();
            return m_instances.create(val);
        };

    private:
        token_type_t m_operator;
        execution_context_t& m_exec_ctx;
        instances_holder_t& m_instances;
    };
}

namespace cmsl
{
    namespace exec
    {
        namespace onp
        {
            onp_executor::onp_executor(const tokens_container_t& onp_tokens, executor& e, int& result)
                : m_tokens{ onp_tokens }
                , m_exec{ e }
                , m_result{ result }
                , m_instances{ m_exec }
            {}

            void onp_executor::execute()
            {
                for (const auto& token : m_tokens)
                {
                    const auto token_type = token.get_type();
                    if (token_type == token_type_t::integer)
                    {
                        const auto val = std::stoi(token.str().to_string());
                        auto inst = m_instances.create(val);
                        m_stack.push(stack_entry_t{ inst });
                    }
                    else if (token_type == token_type_t::identifier)
                    {
                        m_stack.push(stack_entry_t{ token });

                        /*if (const auto var_instance = m_exec.get_exec_ctx().get_variable(token.str()))
                        {
                            m_stack.push(stack_entry_t{ var_instance });
                        }
                        else if(const auto fun = m_exec.get_ast_ctx().find_function(token.str()))
                        {
                            execute_function_call(*fun);
                        }
                        else
                        {
                            // todo raise identifier not found error
                            return;
                        }*/
                    }
                    else
                    {
                        const auto a = get_top_and_pop();
                        const auto b = get_top_and_pop();

                        auto result = apply_operator(b, token_type, a);
                        m_stack.push(stack_entry_t{ result });
                    }
                }

                auto instance = boost::get<inst::instance*>(get_top_and_pop());

                m_result = instance->get_value();
            }

            void onp_executor::execute_function_call(const ast::function_node& fun)
            {
                auto& exec_ctx = m_exec.get_exec_ctx();
                std::vector<inst::instance*> params;

                for (const auto& param_decl : fun.get_params_declarations())
                {
                    auto inst = boost::get<inst::instance*>(get_top_and_pop());
                    params.emplace_back(inst);
                }

                // Parameters are on stack in reverese order
                std::reverse(std::begin(params), std::end(params));

                m_exec.function_call(fun, std::move(params));

                const auto ret_val = m_exec.get_function_return_value();
                auto inst = m_instances.create(ret_val);
                m_stack.push(inst);
            }

            onp_executor::stack_entry_t onp_executor::get_top_and_pop()
            {
                auto entry = std::move(m_stack.top());
                m_stack.pop();
                return std::move(entry);
            }

            inst::instance* onp_executor::apply_operator(const stack_entry_t& lhs, token_type_t op, const stack_entry_t& rhs)
            {
                auto visitor = operator_visitor{ op, m_exec.get_exec_ctx(), m_instances };
                auto result = boost::apply_visitor(visitor, lhs, rhs);

                return result;
            }

            inst::instance_factory onp_executor::get_factory()
            {
                return inst::instance_factory{ m_exec.get_ast_ctx(), m_exec.get_exec_ctx() };
            }

            inst::instance* onp_executor::apply_dot_operator(inst::instance* lhs, inst::instance* rhs)
            {
                return nullptr;
            }

        }
    }
}
