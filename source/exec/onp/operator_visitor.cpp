#include "exec/onp/operator_visitor.hpp"
#include "exec/instance/instance.hpp"
#include "exec/instance/instances_holder.hpp"
#include "exec/execution_context.hpp"
#include "exec/onp/id_access.hpp"
#include "exec/onp/onp_executor.hpp"
#include "common/assert.hpp"

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
                , m_arith_op_handlers {  get_arith_operators_handlers() }
            {}

            operator_visitor::instance_t* operator_visitor::operator()(instance_t* lhs, instance_t* rhs)
            {
                switch(m_operator)
                {
                    case token_type_t::dot:
                        return nullptr; //todo raise error, e.g. (4+2).(2+2)

                    default:
                        return handle_arith_operator(lhs, m_operator, rhs);
                }
            }

            operator_visitor::instance_t* operator_visitor::operator() (instance_t* lhs, id_access& rhs)
            {
                switch(m_operator)
                {
                    case token_type_t::dot:
                        return handle_dot_operator(lhs, rhs);

                    default:
                        return handle_arith_operator(lhs, m_operator, get_instance(rhs));
                }

            }

            operator_visitor::instance_t* operator_visitor::operator()(id_access& lhs, id_access& rhs)
            {
                switch(m_operator)
                {
                    case token_type_t::dot:
                        return handle_dot_operator(get_instance(lhs), rhs);

                    default:
                        return handle_arith_operator(get_instance(lhs), m_operator, get_instance(rhs));
                }
            }

            operator_visitor::instance_t* operator_visitor::operator()(id_access& lhs, instance_t* rhs)
            {
                switch(m_operator)
                {
                    case token_type_t::dot:
                        return nullptr; // todo raise error, e.g. foo.(bar + baz)

                    default:
                        return handle_arith_operator(get_instance(lhs), m_operator, rhs);
                }
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

            operator_visitor::arith_operators_handlers_t operator_visitor::get_arith_operators_handlers()
            {
                return arith_operators_handlers_t{
                        { token_type_t::equal, [](auto lhs, auto rhs)
                                               {
                                                   lhs->assign(rhs->get_value());
                                                   return lhs;
                                               }},
                        { token_type_t::equalequal, [&instances = m_instances](auto lhs, auto rhs)
                                               {
                                                   const auto val = lhs->get_value() == rhs->get_value();
                                                   return instances.create(val);
                                               }},
                        { token_type_t::minus, [&instances = m_instances](auto lhs, auto rhs)
                                               {
                                                   const auto val = lhs->get_value() - rhs->get_value();
                                                   return instances.create(val);
                                               }},
                        { token_type_t::minusequal, [](auto lhs, auto rhs)
                                               {
                                                   const auto val = lhs->get_value() - rhs->get_value();
                                                   lhs->assign(val);
                                                   return lhs;
                                               }},
                        { token_type_t::plus, [&instances = m_instances](auto lhs, auto rhs)
                                               {
                                                   const auto val = lhs->get_value() + rhs->get_value();
                                                   return instances.create(val);
                                               }},
                        { token_type_t::plusequal, [](auto lhs, auto rhs)
                                               {
                                                   const auto val = lhs->get_value() + rhs->get_value();
                                                   lhs->assign(val);
                                                   return lhs;
                                               }},
                        { token_type_t::amp, [&instances = m_instances](auto lhs, auto rhs)
                                               {
                                                   const auto val = lhs->get_value() & rhs->get_value();
                                                   return instances.create(val);
                                               }},
                        { token_type_t::ampamp, [&instances = m_instances](auto lhs, auto rhs)
                                               {
                                                   const auto val = lhs->get_value() && rhs->get_value();
                                                   return instances.create(val);
                                               }},
                        { token_type_t::ampequal, [](auto lhs, auto rhs)
                                               {
                                                   const auto val = lhs->get_value() & rhs->get_value();
                                                   lhs->assign(val);
                                                   return lhs;
                                               }},
                        { token_type_t::pipe, [&instances = m_instances](auto lhs, auto rhs)
                                               {
                                                   const auto val = lhs->get_value() | rhs->get_value();
                                                   return instances.create(val);
                                               }},
                        { token_type_t::pipepipe, [&instances = m_instances](auto lhs, auto rhs)
                                               {
                                                   const auto val = lhs->get_value() || rhs->get_value();
                                                   return instances.create(val);
                                               }},
                        { token_type_t::pipeequal, [](auto lhs, auto rhs)
                                               {
                                                   const auto val = lhs->get_value() | rhs->get_value();
                                                   lhs->assign(val);
                                                   return lhs;
                                               }},
                        { token_type_t::slash, [&instances = m_instances](auto lhs, auto rhs)
                                               {
                                                   const auto val = lhs->get_value() / rhs->get_value();
                                                   return instances.create(val);
                                               }},
                        { token_type_t::slashequal, [](auto lhs, auto rhs)
                                               {
                                                   const auto val = lhs->get_value() / rhs->get_value();
                                                   lhs->assign(val);
                                                   return lhs;
                                               }},
                        { token_type_t::star, [&instances = m_instances](auto lhs, auto rhs)
                                               {
                                                   const auto val = lhs->get_value() * rhs->get_value();
                                                   return instances.create(val);
                                               }},
                        { token_type_t::starequal, [](auto lhs, auto rhs)
                                               {
                                                   const auto val = lhs->get_value() * rhs->get_value();
                                                   lhs->assign(val);
                                                   return lhs;
                                               }},
                        { token_type_t::percent, [&instances = m_instances](auto lhs, auto rhs)
                                               {
                                                   const auto val = lhs->get_value() % rhs->get_value();
                                                   return instances.create(val);
                                               }},
                        { token_type_t::percentequal, [](auto lhs, auto rhs)
                                               {
                                                   const auto val = lhs->get_value() % rhs->get_value();
                                                   lhs->assign(val);
                                                   return lhs;
                                               }},
                        { token_type_t::exclaimequal, [&instances = m_instances](auto lhs, auto rhs)
                                               {
                                                   const auto val = lhs->get_value() != rhs->get_value();
                                                   return instances.create(val);
                                               }},
                        { token_type_t::xor_, [&instances = m_instances](auto lhs, auto rhs)
                                               {
                                                   const auto val = lhs->get_value() ^ rhs->get_value();
                                                   return instances.create(val);
                                               }},
                        { token_type_t::xorequal, [](auto lhs, auto rhs)
                                               {
                                                   const auto val = lhs->get_value() ^ rhs->get_value();
                                                   lhs->assign(val);
                                                   return lhs;
                                               }},
                        { token_type_t::less, [&instances = m_instances](auto lhs, auto rhs)
                                               {
                                                   const auto val = lhs->get_value() < rhs->get_value();
                                                   return instances.create(val);
                                               }},
                        { token_type_t::lessequal, [&instances = m_instances](auto lhs, auto rhs)
                                               {
                                                   const auto val = lhs->get_value() <= rhs->get_value();
                                                   return instances.create(val);
                                               }},
                        { token_type_t::greater, [&instances = m_instances](auto lhs, auto rhs)
                                               {
                                                   const auto val = lhs->get_value() > rhs->get_value();
                                                   return instances.create(val);
                                               }},
                        { token_type_t::greaterequal, [&instances = m_instances](auto lhs, auto rhs)
                                               {
                                                   const auto val = lhs->get_value() >= rhs->get_value();
                                                   return instances.create(val);
                                               }}
                    };
                }

            operator_visitor::instance_t * operator_visitor::handle_arith_operator(instance_t *lhs, token_type_t  op, instance_t *rhs)
            {
                auto found_handler_it = m_arith_op_handlers.find(op);

                if(found_handler_it == m_arith_op_handlers.end())
                {
                    CMSL_UNREACHABLE("Applying not supported operator");
                    return nullptr;
                }

                const auto& handler = found_handler_it->second;
                return handler(lhs, rhs);
            }
        }
    }
}
