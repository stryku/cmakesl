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
                switch(m_operator)
                {
                    case token_type_t::plus: return handle_plus_operator(lhs, rhs);
                    case token_type_t::dot: return nullptr; //todo raise error
                }
            }

            operator_visitor::instance_t* operator_visitor::operator() (instance_t* lhs, const token_t& rhs)
            {
                switch(m_operator)
                {
                    case token_type_t::plus: return handle_plus_operator(lhs, get_instance(rhs));
                    case token_type_t::dot: return handle_dot_operator(lhs, rhs);
                }
            }

            operator_visitor::instance_t* operator_visitor::operator()(const token_t& lhs, const token_t& rhs)
            {
                switch(m_operator)
                {
                    case token_type_t::plus: return handle_plus_operator(get_instance(lhs), get_instance(rhs));
                    case token_type_t::dot: return handle_dot_operator(get_instance(lhs), rhs);
                }
            }

            operator_visitor::instance_t* operator_visitor::operator()(const token_t& lhs, instance_t* rhs)
            {
                switch(m_operator)
                {
                    case token_type_t::plus: return handle_plus_operator(get_instance(lhs), rhs);
                    case token_type_t::dot: return nullptr; // todo raise error
                }
            }

            operator_visitor::instance_t* operator_visitor::handle_plus_operator(instance_t* lhs, instance_t* rhs)
            {
                const auto val = lhs->get_value() + rhs->get_value();
                return m_instances.create(val);
            }

            operator_visitor::instance_t* operator_visitor::handle_dot_operator(instance_t* lhs, const token_t& rhs)
            {
                return lhs->get_member(rhs.str());
            }

            operator_visitor::instance_t* operator_visitor::get_instance(const token_t& token)
            {
                return m_exec_ctx.get_variable(token.str());
            }
        }
    }
}
