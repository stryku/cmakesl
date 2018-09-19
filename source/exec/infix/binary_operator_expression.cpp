#include "exec/infix/binary_operator_expression.hpp"

#include "exec/onp/operator_evaluation_visitor.hpp"
#include "infix_evaluation_context.hpp"
#include "exec/instance/instances_holder.hpp"

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
            {}

            inst::instance* binary_operator_expression::evaluate(infix_evaluation_context &ctx) const
            {
                auto lhs_inst = m_lhs->evaluate(ctx);
                auto rhs_inst = m_rhs->evaluate(ctx);

                auto result = apply_operator_visitor(lhs_inst, m_operator.get_type(), rhs_inst);
                return ctx.instances.create(std::move(result));
            }

            inst::instance_value_t
            binary_operator_expression::apply_operator_visitor(inst::instance *lhs, token_type_t op, inst::instance *rhs) const
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
        }
    }
}



