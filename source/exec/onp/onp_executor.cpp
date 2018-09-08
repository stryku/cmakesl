#include "exec/onp/onp_executor.hpp"
#include "exec/execution.hpp"
#include "exec/instance/named_instance.hpp"
#include "ast/ast_context.hpp"
#include "exec/instance/instance_factory.hpp"
#include "exec/onp/operator_visitor.hpp"


namespace cmsl
{
    namespace exec
    {
        namespace onp
        {
            onp_executor::onp_executor(const tokens_container_t& onp_tokens, execution& e, inst::instance_value_t& result)
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
                        const auto val = std::stoll(token.str().to_string());
                        auto inst = m_instances.create(static_cast<inst::int_t>(val));
                        m_stack.push(stack_entry_t{ inst });
                    }
                    else if(token_type == token_type_t::string)
                    {
                        const auto val = token.str().to_string();
                        auto inst = m_instances.create(val);
                        m_stack.push(stack_entry_t{ inst });
                    }
                    else if (token_type == token_type_t::identifier)
                    {
                        m_stack.push(stack_entry_t{ id_access{nullptr, token.str() } });
                    }
                    else
                    {
                        auto a = get_top_and_pop();
                        auto b = get_top_and_pop();

                        auto result = apply_operator(b, token_type, a);
                        m_stack.push(stack_entry_t{ result });
                    }
                }

                if(!m_stack.empty())
                {
                    auto instance = get_instance_from_stack_top();
                    m_result = instance->get_value();
                }
            }

            inst::instance* onp_executor::get_instance_from_stack_top()
            {
                class visitor : public boost::static_visitor<inst::instance*>
                {
                public:
                    visitor(execution_context& exec_ctx)
                        : m_exec_ctx{ exec_ctx }
                    {}

                    inst::instance* operator()(inst::instance* i) const
                    {
                        return i;
                    }

                    inst::instance* operator()(id_access& access) const
                    {
                        return access.get_instance(m_exec_ctx);
                    }

                private:
                    execution_context& m_exec_ctx;
                };

                auto top = get_top_and_pop();
                return boost::apply_visitor(visitor{ m_exec.get_exec_ctx() }, top);
            }

            std::vector<inst::instance*> onp_executor::prepare_parameters_for_call(const ast::function& fun)
            {
                auto& exec_ctx = m_exec.get_exec_ctx();
                std::vector<inst::instance*> params;

                for (const auto& param_decl : fun.get_params_declarations())
                {
                    auto inst = get_instance_from_stack_top();
                    params.emplace_back(inst);
                }

                // Parameters are popped from stack in reverse order
                std::reverse(std::begin(params), std::end(params));

                return params;
            }

            inst::instance* onp_executor::execute_member_function_call(inst::instance* class_instance, cmsl::string_view name)
            {
                if(class_instance->get_type().is_builtin())
                {
                    const auto& fun = *class_instance->get_function(name);
                    auto params = prepare_parameters_for_call(fun);
                    auto ret_val = m_exec.fundamental_member_function_call(class_instance, name, std::move(params));
                    return m_instances.create(ret_val);
                }
                else
                {
                    const auto& fun = *dynamic_cast<const ast::user_function_node*>(class_instance->get_function(name));
                    auto params = prepare_parameters_for_call(fun);
                    m_exec.member_function_call(fun, std::move(params), class_instance);

                    const auto ret_val = m_exec.get_function_return_value();
                    return m_instances.create(ret_val);
                }
            }

            inst::instance* onp_executor::execute_function_call(const ast::user_function_node& fun)
            {
                auto params = prepare_parameters_for_call(fun);
                m_exec.function_call(fun, std::move(params));

                const auto ret_val = m_exec.get_function_return_value();
                return m_instances.create(ret_val);
            }

            onp_executor::stack_entry_t onp_executor::get_top_and_pop()
            {
                auto entry = std::move(m_stack.top());
                m_stack.pop();
                return std::move(entry);
            }

            inst::instance* onp_executor::apply_operator(stack_entry_t& lhs, token_type_t op, stack_entry_t& rhs)
            {
                auto visitor = operator_visitor{ op, m_exec.get_exec_ctx(), m_instances, *this };
                return boost::apply_visitor(visitor, lhs, rhs);
            }
        }
    }
}
