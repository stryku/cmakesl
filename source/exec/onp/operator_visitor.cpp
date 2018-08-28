#include "exec/onp/operator_visitor.hpp"
#include "exec/instance/instance.hpp"
#include "exec/instance/instances_holder.hpp"
#include "exec/execution_context.hpp"
#include "exec/onp/id_access.hpp"
#include "exec/onp/onp_executor.hpp"

namespace cmsl
{
    namespace exec
    {
        namespace onp
        {
            operator_visitor::operator_visitor(token_type_t op,
                                               execution_context_t& exec_ctx,
                                               instances_holder_t& instances,
                                               onp_executor& function_caller)
                : m_operator{ op }
                , m_exec_ctx{ exec_ctx }
                , m_instances{ instances }
                , m_function_caller{ function_caller }
            {}

            operator_visitor::instance_t* operator_visitor::operator()(instance_t* lhs, instance_t* rhs)
            {
                switch(m_operator)
                {
                    case token_type_t::plus:
                        return handle_plus_operator(lhs, rhs);
                    case token_type_t::dot:
                        return nullptr; //todo raise error
                    case token_type_t::equal:
                        return handle_equal_operator(lhs, rhs);
                }
            }

            operator_visitor::instance_t* operator_visitor::operator() (instance_t* lhs, id_access& rhs)
            {
                switch(m_operator)
                {
                    case token_type_t::plus:
                        return handle_plus_operator(lhs, get_instance(rhs));
                    case token_type_t::dot:
                        return handle_dot_operator(lhs, rhs);
                    case token_type_t::equal:
                        return handle_equal_operator(lhs, get_instance(rhs));
                }

            }

            operator_visitor::instance_t* operator_visitor::operator()(id_access& lhs, id_access& rhs)
            {
                switch(m_operator)
                {
                    case token_type_t::plus:
                        return handle_plus_operator(get_instance(lhs), get_instance(rhs));
                    case token_type_t::dot:
                        return handle_dot_operator(get_instance(lhs), rhs);
                    case token_type_t::equal:
                        return handle_equal_operator(get_instance(lhs), get_instance(rhs));
                }
            }

            operator_visitor::instance_t* operator_visitor::operator()(id_access& lhs, instance_t* rhs)
            {
                switch(m_operator)
                {
                    case token_type_t::plus:
                        return handle_plus_operator(get_instance(lhs), rhs);
                    case token_type_t::dot:
                        return nullptr; // todo raise error
                    case token_type_t::equal:
                        return handle_equal_operator(get_instance(lhs), rhs);
                }
            }

            operator_visitor::instance_t* operator_visitor::handle_plus_operator(instance_t* lhs, instance_t* rhs)
            {
                const auto val = lhs->get_value() + rhs->get_value();
                return m_instances.create(val);
            }

            operator_visitor::instance_t* operator_visitor::handle_dot_operator(instance_t* lhs, id_access& rhs)
            {
                if(lhs->has_function(rhs.name))
                {
                    return m_function_caller.execute_member_function_call(*lhs->get_function(rhs.name), lhs);
                }
                else
                {
                    return lhs->get_member(rhs.name);
                }
            }

            operator_visitor::instance_t* operator_visitor::get_instance(id_access& access)
            {
                return access.get_instance(m_exec_ctx);
            }

            operator_visitor::instance_t* operator_visitor::handle_equal_operator(instance_t* lhs, instance_t* rhs)
            {
                lhs->assign(rhs->get_value());
                return lhs;
            }
        }
    }
}
