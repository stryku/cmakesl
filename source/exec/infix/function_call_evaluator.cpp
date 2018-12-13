#include "function_call_evaluator.hpp"

#include "common/assert.hpp"
#include "ast/ast_context.hpp"
#include "ast/builtin_function.hpp"
#include "exec/execution.hpp"
#include "exec/builtin/builtin_function_caller.hpp"
#include "infix_evaluation_context.hpp"
#include "exec/instance/instances_holder.hpp"
#include "exec/instance/instance.hpp"
#include "exec/builtin/builtin_function_factory.hpp"
#include "exec/builtin/evaluatable_function.hpp"

namespace cmsl
{
    namespace exec
    {
        namespace infix
        {
            function_call_evaluator::function_call_evaluator(builtin::builtin_function_factory& function_factory, infix_evaluation_context& ctx, execution &e)
                : m_ctx{ ctx }
                    , m_execution{ e }
                    , m_function_factory{ function_factory }
            {}

            inst::instance *
            function_call_evaluator::call(const ast::function &fun, const std::vector<inst::instance *> &params)
            {
                const auto& ast_ctx = m_ctx.m_ctx_provider.get_ast_ctx();
                if(auto type = ast_ctx.find_type(fun.get_name()))
                {
                    // Calling a constructor.
                    auto class_instance = m_ctx.instances.create(*type);
                    return call_member(*class_instance, fun, params);
                }
                else if (auto user_function = dynamic_cast<const ast::user_function_node *>(&fun))
                {
                    m_execution.function_call(*user_function, params);
                    return get_and_store_function_result();
                }
                else if(auto builtin_fun = dynamic_cast<const ast::builtin_function*>(&fun))
                {
                    // builtin function
                    auto eval_function = m_function_factory.create(m_ctx.m_ctx_provider.get_ast_ctx(), m_ctx.instances, builtin_fun->get_fundamental_fun_kind(), params);
                    return eval_function->evaluate();
                }

                CMSL_UNREACHABLE("Call of neither user nor builtin function");
            }

            inst::instance *
            function_call_evaluator::call_member(inst::instance &class_instance, const ast::function &fun, const std::vector<inst::instance *> &params)
            {
                if(class_instance.get_type().is_builtin())
                {
                    builtin::builtin_function_caller caller{ m_ctx.instances, m_execution.get_cmake_facade() };
                    const auto casted_fun = dynamic_cast<const ast::builtin_function*>(&fun);
                    return caller.call_member_function(&class_instance, *casted_fun, params);
                }
                else
                {
                    const auto& casted_fun = *dynamic_cast<const ast::user_function_node*>(&fun);
                    m_execution.member_function_call(casted_fun, std::move(params), &class_instance);

                    // If just executed function was a constructor, return class instance. It's already stored in ctx.instances.
                    if(class_instance.is_ctor(fun.get_name()))
                    {
                        return &class_instance;
                    }

                    return get_and_store_function_result();
                }
            }

            inst::instance *function_call_evaluator::get_and_store_function_result()
            {
                auto result = m_execution.get_function_return_value();
                auto result_ptr = result.get();
                m_ctx.instances.store(std::move(result));
                return result_ptr;
            }
        }
    }
}