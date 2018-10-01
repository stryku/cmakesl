#include "exec/onp/onp_executor.hpp"
#include "exec/execution.hpp"
#include "exec/instance/named_instance.hpp"
#include "ast/ast_context.hpp"
#include "exec/instance/instance_factory.hpp"
#include "exec/infix/operator_visitor.hpp"

namespace cmsl
{
    namespace exec
    {
        namespace onp
        {
            onp_executor::onp_executor(const tokens_container_t& onp_tokens, execution& e)
                : m_tokens{ onp_tokens }
                , m_exec{ e }
                , m_instances{ m_exec }
                ,m_builtin_fun_caller{ m_instances }
            {}

            std::unique_ptr<inst::instance> onp_executor::execute()
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
                        if(is_function_or_ctor(token))
                        {
                            auto result = handle_function_call(token);
                            m_stack.push(stack_entry_t{ result });
                        }
                        else
                        {
                            m_stack.push(stack_entry_t{ id_access{nullptr, token.str() } });
                        }
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
                    return instance->copy();
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
                    return m_builtin_fun_caller.call_member_function(class_instance, name, std::move(params));
                }
                else
                {
                    const auto& fun = *dynamic_cast<const ast::user_function_node*>(class_instance->get_function(name));
                    auto params = prepare_parameters_for_call(fun);
                    m_exec.member_function_call(fun, std::move(params), class_instance);

                    // If just executed function was a constructor, return class instance. It's already stored in m_instances.
                    if(class_instance->is_ctor(name))
                    {
                        return class_instance;
                    }

                    auto ret_instance = m_exec.get_function_return_value();
                    auto inst_ptr = ret_instance.get();
                    m_instances.store(std::move(ret_instance));
                    return inst_ptr;
                }
            }

            inst::instance* onp_executor::execute_function_call(const ast::user_function_node& fun)
            {
                auto params = prepare_parameters_for_call(fun);
                m_exec.function_call(fun, std::move(params));

                auto ret_instance = m_exec.get_function_return_value();
                auto ret_instance_ptr = ret_instance.get();
                m_instances.store(std::move(ret_instance));
                return ret_instance_ptr;
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

            bool onp_executor::is_function_or_ctor(const token_t& tok) const
            {
                const auto& ctx = m_exec.get_ast_ctx();
                return ctx.find_function(tok.str()) ||
                       ctx.find_type(tok.str()); // Class ctors are threated as functions at this point
            }

            inst::instance *onp_executor::handle_function_call(const onp_executor::token_t &tok)
            {
                const auto name = tok.str();

                const auto& ctx = m_exec.get_ast_ctx();

                if(const auto fun = ctx.find_function(name))
                {
                    auto casted = dynamic_cast<const ast::user_function_node*>(fun);
                    execute_function_call(*casted);
                }
                else if(const auto ctor_type = ctx.find_type(name))
                {
                    auto instance = m_instances.create(*ctor_type);

                    const auto ctor_function = ctor_type->get_function(name);

                    // No defined ctor. Return default instance value.
                    if(!ctor_function)
                    {
                        return instance;
                    }
                    else
                    {
                        return execute_member_function_call(instance, name);
                    }
                }
            }
        }
    }
}
