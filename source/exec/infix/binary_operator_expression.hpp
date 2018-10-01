#pragma once

#include "common/enum_class_utils.hpp"
#include "exec/infix/infix_expression.hpp"
#include "exec/instance/instance_value.hpp"
#include "lexer/token/token.hpp"
#include "exec/instance/instance_value.hpp"


#include <memory>

namespace cmsl
{
    namespace exec
    {
        namespace inst
        {
            class instances_holder;
        }

        namespace infix
        {
            class binary_operator_expression : public infix_expression
            {
            private:
                using token_type_t = lexer::token::token_type;
                using instance_t = inst::instance;

                using arith_operator_handler_t = std::function<instance_t*(inst::instances_holder& instances, instance_t*, instance_t*)>;
                using arith_operators_handlers_t = enum_unordered_map<token_type_t, arith_operator_handler_t>;

            public:
                binary_operator_expression(std::unique_ptr<infix_expression> lhs, lexer::token::token op,  std::unique_ptr<infix_expression> rhs);

                instance_t* evaluate(infix_evaluation_context& ctx) const override;
                void visit(infix_expression_visitor& visitor) const override;

            private:
                //todo remove static
                static inst::instance_value_t apply_operator_visitor(instance_t* lhs, token_type_t  op, instance_t* rhs);

                instance_t* handle_arith_operator(inst::instances_holder& instances, instance_t* lhs, token_type_t  op, instance_t* rhs) const;

                arith_operators_handlers_t get_arith_operators_handlers();
                arith_operator_handler_t get_arith_operator_handler(token_type_t op);
                arith_operator_handler_t get_compound_assignment_handler(token_type_t op);

                token_type_t map_compound_assignment_op(token_type_t op);

            private:
                std::unique_ptr<infix_expression> m_lhs;
                lexer::token::token m_operator;
                std::unique_ptr<infix_expression> m_rhs;
                arith_operators_handlers_t m_arith_op_handlers;
            };
        }
    }
}



