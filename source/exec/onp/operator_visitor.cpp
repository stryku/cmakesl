#include "exec/onp/operator_visitor.hpp"
#include "exec/instance/instance.hpp"
#include "exec/instance/instances_holder.hpp"
#include "exec/execution_context.hpp"
#include "exec/onp/id_access.hpp"
#include "exec/onp/onp_executor.hpp"
#include "common/assert.hpp"
#include "exec/onp/operator_evaluation_visitor.hpp"

namespace cmsl
{
    namespace exec
    {
        namespace onp
        {
            operator_visitor::operator_visitor(token_type_t op,
                                               execution_context_t &exec_ctx,
                                               instances_holder_t &instances,
                                               onp_executor &function_caller)
                    : m_operator{op}
                    , m_exec_ctx{exec_ctx}
                    , m_instances{instances}
                    , m_function_caller{function_caller}
                    , m_arith_op_handlers{get_arith_operators_handlers()}
            {}

            operator_visitor::instance_t *operator_visitor::operator()(instance_t *lhs, instance_t *rhs)
            {
                switch (m_operator)
                {
                    case token_type_t::dot:
                        return nullptr; //todo raise error, e.g. (4+2).(2+2)

                    default:
                        return handle_arith_operator(lhs, m_operator, rhs);
                }
            }

            operator_visitor::instance_t *operator_visitor::operator()(instance_t *lhs, id_access &rhs)
            {
                switch (m_operator)
                {
                    case token_type_t::dot:
                        return handle_dot_operator(lhs, rhs);

                    default:
                        return handle_arith_operator(lhs, m_operator, get_instance(rhs));
                }

            }

            operator_visitor::instance_t *operator_visitor::operator()(id_access &lhs, id_access &rhs)
            {
                switch (m_operator)
                {
                    case token_type_t::dot:
                        return handle_dot_operator(get_instance(lhs), rhs);

                    default:
                        return handle_arith_operator(get_instance(lhs), m_operator, get_instance(rhs));
                }
            }

            operator_visitor::instance_t *operator_visitor::operator()(id_access &lhs, instance_t *rhs)
            {
                switch (m_operator)
                {
                    case token_type_t::dot:
                        return nullptr; // todo raise error, e.g. foo.(bar + baz)

                    default:
                        return handle_arith_operator(get_instance(lhs), m_operator, rhs);
                }
            }

            operator_visitor::instance_t *operator_visitor::handle_dot_operator(instance_t *lhs, id_access &rhs)
            {
                if (lhs->has_function(rhs.name))
                {
                    return m_function_caller.execute_member_function_call(*lhs->get_function(rhs.name), lhs);
                }
                else
                {
                    return lhs->get_member(rhs.name);
                }
            }

            operator_visitor::instance_t *operator_visitor::get_instance(id_access &access)
            {
                return access.get_instance(m_exec_ctx);
            }

            operator_visitor::arith_operators_handlers_t operator_visitor::get_arith_operators_handlers()
            {
                const auto simple_operators = {
                        token_type_t::minus,
                        token_type_t::plus,
                        token_type_t::amp,
                        token_type_t::ampamp,
                        token_type_t::pipe,
                        token_type_t::pipepipe,
                        token_type_t::slash,
                        token_type_t::star,
                        token_type_t::percent,
                        token_type_t::exclaimequal,
                        token_type_t::xor_,
                        token_type_t::less,
                        token_type_t::lessequal,
                        token_type_t::greater,
                        token_type_t::greaterequal
                };

                const auto compound_assignment_operators = {
                        token_type_t::minusequal,
                        token_type_t::plusequal,
                        token_type_t::ampequal,
                        token_type_t::pipeequal,
                        token_type_t::slashequal,
                        token_type_t::starequal,
                        token_type_t::percentequal,
                        token_type_t::xorequal
                };

                arith_operators_handlers_t handlers;

                for (const auto op : simple_operators)
                {
                    handlers.emplace(op, get_arith_operator_handler(op));
                }

                for (const auto op : compound_assignment_operators)
                {
                    handlers.emplace(op, get_compound_assignment_handler(op));
                }

                handlers.emplace(token_type_t::equal, [](auto lhs, auto rhs)
                {
                    lhs->assign(rhs->get_value());
                    return lhs;
                });

                return handlers;
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

            operator_visitor::arith_operator_handler_t operator_visitor::get_arith_operator_handler(token_type_t op)
            {
                return [&, op](auto lhs, auto rhs)
                {
                    auto result = apply_operator_visitor(lhs, op, rhs);
                    return m_instances.create(result);
                };
            }

            operator_visitor::arith_operator_handler_t
            operator_visitor::get_compound_assignment_handler(operator_visitor::token_type_t op)
            {
                const auto eval_operator = map_compound_assignment_op(op);
                return [&, eval_operator](auto lhs, auto rhs)
                {
                    const auto result = apply_operator_visitor(lhs, eval_operator, rhs);
                    lhs->assign(result);
                    return lhs;
                };
            }

            inst::instance_value_t operator_visitor::apply_operator_visitor(instance_t *lhs, token_type_t op, instance_t *rhs)
            {
                auto lhs_val = lhs->get_value();
                auto rhs_val = rhs->get_value();

                switch(op)
                {

                    case token_type_t::minus:
                    case token_type_t::plus:
                    {
                        auto visitor = additive_operator_evaluation_visitor{ op };
                        return boost::apply_visitor(visitor, lhs_val, rhs_val);
                    }

                    case token_type_t::amp:
                    case token_type_t::pipe:
                    case token_type_t::xor_:
                    {
                        auto visitor = bitwise_operator_evaluation_visitor{ op };
                        return boost::apply_visitor(visitor, lhs_val, rhs_val);
                    }

                    case token_type_t::slash:
                    case token_type_t::star:
                    {
                        auto visitor = multiplicative_operator_evaluation_visitor{ op };
                        return boost::apply_visitor(visitor, lhs_val, rhs_val);
                    }

                    case token_type_t::percent:
                    {
                        auto visitor = reminder_operator_evaluation_visitor{ op };
                        return boost::apply_visitor(visitor, lhs_val, rhs_val);
                    }

                    case token_type_t::ampamp:
                    case token_type_t::pipepipe:
                    case token_type_t::equalequal:
                    case token_type_t::exclaimequal:
                    case token_type_t::less:
                    case token_type_t::lessequal:
                    case token_type_t::greater:
                    case token_type_t::greaterequal:
                    {
                        auto visitor = logical_operator_evaluation_visitor{ op };
                        return boost::apply_visitor(visitor, lhs_val, rhs_val);
                    }
                }
            }

            operator_visitor::token_type_t
            operator_visitor::map_compound_assignment_op(operator_visitor::token_type_t op)
            {
                switch(op)
                {

                    case token_type_t::minusequal: return token_type_t::minus ;
                    case token_type_t::plusequal: return token_type_t::plus;
                    case token_type_t::ampequal: return token_type_t::amp;
                    case token_type_t::pipeequal: return token_type_t::pipe;
                    case token_type_t::slashequal: return token_type_t::slash;
                    case token_type_t::starequal: return token_type_t::star;
                    case token_type_t::percentequal: return token_type_t::percent;
                    case token_type_t::xorequal: return token_type_t::xor_;

                    default:
                        CMSL_UNREACHABLE("op is not a compound assignment operator");
                        return op;
                }
            }
        }
    }
}
