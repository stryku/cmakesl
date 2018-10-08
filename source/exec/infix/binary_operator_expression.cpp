#include "exec/infix/binary_operator_expression.hpp"

#include "operator_evaluation_visitor.hpp"
#include "infix_evaluation_context.hpp"
#include "exec/instance/instances_holder.hpp"
#include "exec/infix/infix_expression_visitor.hpp"

namespace cmsl
{
    namespace exec
    {
        namespace infix
        {
            binary_operator_expression::binary_operator_expression(std::unique_ptr<infix_expression> lhs, lexer::token::token op, std::unique_ptr<infix_expression> rhs)
                : m_lhs{ std::move(lhs) }
                , m_operator{ op }
                , m_rhs{ std::move(rhs) }
                , m_arith_op_handlers{ get_arith_operators_handlers() }
            {}

            inst::instance* binary_operator_expression::evaluate(infix_evaluation_context &ctx) const
            {
                auto lhs_inst = m_lhs->evaluate(ctx);
                auto rhs_inst = m_rhs->evaluate(ctx);

                auto result = handle_arith_operator(ctx.instances,
                                                    lhs_inst,
                                                    m_operator.get_type(),
                                                    rhs_inst);
                return result;
            }

            inst::instance_value_t
            binary_operator_expression::apply_operator_visitor(inst::instance *lhs, token_type_t op, inst::instance *rhs)
            {
                auto lhs_val = lhs->get_value();
                auto rhs_val = rhs->get_value();

                switch(op)
                {
                    case token_type_t::minus:
                    {
                        auto visitor = onp::minus_operator_evaluation_visitor{};
                        return visitor.visit(lhs_val, rhs_val);
                    }

                    case token_type_t::plus:
                    {
                        auto visitor = onp::plus_operator_evaluation_visitor{};
                        return visitor.visit(lhs_val, rhs_val);
                    }

                    case token_type_t::amp:
                    case token_type_t::pipe:
                    case token_type_t::xor_:
                    {
                        auto visitor = onp::bitwise_operator_evaluation_visitor{ op };
                        return visitor.visit(lhs_val, rhs_val);
                    }

                    case token_type_t::slash:
                    case token_type_t::star:
                    {
                        auto visitor = onp::multiplicative_operator_evaluation_visitor{ op };
                        return visitor.visit(lhs_val, rhs_val);
                    }

                    case token_type_t::percent:
                    {
                        auto visitor = onp::reminder_operator_evaluation_visitor{};
                        return visitor.visit(lhs_val, rhs_val);
                    }

                    case token_type_t::less:
                    case token_type_t::lessequal:
                    case token_type_t::greater:
                    case token_type_t::greaterequal:
                    {
                        auto visitor = onp::relation_operator_evaluation_visitor{ op };
                        return visitor.visit(lhs_val, rhs_val);
                    }

                    case token_type_t::equalequal:
                    case token_type_t::exclaimequal:
                    {
                        auto visitor = onp::comparison_operator_evaluation_visitor{ op };
                        return visitor.visit(lhs_val, rhs_val);
                    }

                    case token_type_t::pipepipe:
                    case token_type_t::ampamp:
                    {
                        auto visitor = onp::logical_operator_evaluation_visitor{ op };
                        return visitor.visit(lhs_val, rhs_val);
                    }
                }
            }

            void binary_operator_expression::visit(infix_expression_visitor &visitor) const
            {
                return visitor.visit(*this);
            }

            binary_operator_expression::arith_operators_handlers_t binary_operator_expression::get_arith_operators_handlers()
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
                        token_type_t::greaterequal,
                        token_type_t::equalequal
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

                handlers.emplace(token_type_t::equal, [](auto&, auto lhs, auto rhs)
                {
                    lhs->assign(rhs->get_value());
                    return lhs;
                });

                return handlers;
            }

            binary_operator_expression::instance_t * binary_operator_expression::handle_arith_operator(inst::instances_holder& instances, instance_t *lhs, token_type_t  op, instance_t *rhs) const
            {
                auto found_handler_it = m_arith_op_handlers.find(op);

                if(found_handler_it == m_arith_op_handlers.end())
                {
                    CMSL_UNREACHABLE("Applying not supported operator");
                    return nullptr;
                }

                const auto& handler = found_handler_it->second;
                return handler(instances, lhs, rhs);
            }

            binary_operator_expression::arith_operator_handler_t binary_operator_expression::get_arith_operator_handler(token_type_t op)
            {
                return [this, op](auto& instances, auto lhs, auto rhs)
                {
                    auto result = apply_operator_visitor(lhs, op, rhs);
                    return instances.create(result);
                };
            }

            binary_operator_expression::arith_operator_handler_t
            binary_operator_expression::get_compound_assignment_handler(token_type_t op)
            {
                const auto eval_operator = map_compound_assignment_op(op);
                return [&, eval_operator](auto& instances, auto lhs, auto rhs)
                {
                    const auto result = apply_operator_visitor(lhs, eval_operator, rhs);
                    lhs->assign(result);
                    return lhs;
                };
            }

            binary_operator_expression::token_type_t
            binary_operator_expression::map_compound_assignment_op(token_type_t op)
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

            const infix_expression &binary_operator_expression::get_lhs() const
            {
                return *m_lhs;
            }

            lexer::token::token binary_operator_expression::get_operator() const
            {
                return m_operator;
            }

            const infix_expression &binary_operator_expression::get_rhs() const
            {
                return *m_rhs;
            }
        }
    }
}



