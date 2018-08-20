#include "exec/onp/operator_visitor.hpp"
#include "exec/instance/instance.hpp"
#include "exec/instance/instances_holder.hpp"
#include "exec/execution_context.hpp"

namespace cmsl
{
    namespace exec
    {
        namespace onp
        {
            operator_visitor::operator_visitor(token_type_t op,
                                               execution_context_t& exec_ctx,
                                               instances_holder_t& instances)
                : m_operator{ op }
                , m_exec_ctx{ exec_ctx }
                , m_instances{ instances }
            {}

            operator_visitor::instance_t* operator_visitor::operator()(instance_t* lhs, instance_t* rhs)
            {
                // todo handle all operators
                const auto val = lhs->get_value() + rhs->get_value();
                return m_instances.create(val);
            }

            operator_visitor::instance_t* operator_visitor::operator() (instance_t* lhs, const token_t& token)
            {
                // todo handle all operators
                return lhs->get_member(token.str());
            }

            operator_visitor::instance_t* operator_visitor::operator()(const token_t& lhs, const token_t& rhs)
            {
                // todo handle all operators
                auto lhs_instance = m_exec_ctx.get_variable(lhs.str());
                return lhs_instance->get_member(rhs.str());
            }

            operator_visitor::instance_t* operator_visitor::operator()(const token_t& lhs, instance_t* rhs)
            {
                // todo handle all operators
                auto lhs_instance = m_exec_ctx.get_variable(lhs.str());
                const auto val = lhs_instance->get_value() + rhs->get_value();
                return m_instances.create(val);
            }
        }
    }
}
