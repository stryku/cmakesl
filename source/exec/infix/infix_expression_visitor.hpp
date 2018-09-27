#pragma once

namespace cmsl
{
    namespace exec
    {
        namespace inst
        {
            class instance;
        }

        namespace infix
        {
            class bool_value_expression;
            class int_value_expression;
            class double_value_expression;
            class string_value_expression;
            class id_expression;
            class member_function_call_expression;
            class binary_operator_expression;
            class function_call_expression;
            class class_member_access_expression;

            class infix_expression_visitor
            {
            public:
                virtual ~infix_expression_visitor() = default;

                virtual void visit(const bool_value_expression& expr) = 0;
                virtual void visit(const int_value_expression& expr) = 0;
                virtual void visit(const double_value_expression& expr) = 0;
                virtual void visit(const string_value_expression& expr) = 0;
                virtual void visit(const id_expression& expr) = 0;
                virtual void visit(const member_function_call_expression& expr) = 0;
                virtual void visit(const binary_operator_expression& expr) = 0;
                virtual void visit(const function_call_expression& expr) = 0;
                virtual void visit(const class_member_access_expression& expr) = 0;
            };
        }
    }
}
