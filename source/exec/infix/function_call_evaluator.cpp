#include "function_call_evaluator.hpp"

#include "common/assert.hpp"
#include "ast/ast_context.hpp"
#include "ast/fundamental_function.hpp"
#include "exec/execution.hpp"
#include "exec/builtin_function_caller.hpp"
#include "infix_evaluation_context.hpp"
#include "exec/instance/instances_holder.hpp"
#include "exec/instance/instance.hpp"

namespace cmsl
{
    namespace exec
    {
        namespace infix
        {
            function_call_evaluator::function_call_evaluator(infix_evaluation_context& ctx, execution &e)
                : m_ctx{ ctx }
                , m_execution{ e }
            {}

            inst::instance *
            function_call_evaluator::call(const ast::function &fun, const std::vector<inst::instance *> &params)
            {
                if (auto user_function = dynamic_cast<const ast::user_function_node *>(&fun))
                {
                    m_execution.function_call(*user_function, params);
                    return get_and_store_function_result();
                }
                else if(auto builtin_fun = dynamic_cast<const ast::fundamental_function*>(&fun))
                {
                    // builtin function
                    builtin_function_caller caller{ m_ctx.instances };
                    return caller.call(builtin_fun->get_fundamental_fun_kind(), params);
                }

                CMSL_UNREACHABLE("Call of a neither user nor builtin function");
            }

            inst::instance *
            function_call_evaluator::call_member(inst::instance &class_instance, const ast::function &fun, const std::vector<inst::instance *> &params)
            {
                if(class_instance.get_type().is_builtin())
                {
                    builtin_function_caller caller{ m_ctx.instances };
                    const auto casted_fun = dynamic_cast<const ast::fundamental_function*>(&fun);
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