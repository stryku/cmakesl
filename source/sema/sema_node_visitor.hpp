#pragma once

namespace cmsl
{
    namespace sema
    {
        class variable_declaration_node;
        class bool_value_node;
        class int_value_node;
        class double_value_node;
        class string_value_node;
        class id_node;
        class return_node;
        class binary_operator_node;
        class function_call_node;
        class member_function_call_node;
        class block_node;
        class function_node;
        class class_node;
        class conditional_node;
        class while_node;

        class sema_node_visitor
        {
        public:
            virtual ~sema_node_visitor() = default;

            virtual void visit(const variable_declaration_node& node) = 0;
            virtual void visit(const bool_value_node& node) = 0;
            virtual void visit(const int_value_node& node) = 0;
            virtual void visit(const double_value_node& node) = 0;
            virtual void visit(const string_value_node& node) = 0;
            virtual void visit(const id_node& node) = 0;
            virtual void visit(const return_node& node) = 0;
            virtual void visit(const binary_operator_node& node) = 0;
            virtual void visit(const function_call_node& node) = 0;
            virtual void visit(const member_function_call_node& node) = 0;
            virtual void visit(const block_node& node) = 0;
            virtual void visit(const function_node& node) = 0;
            virtual void visit(const class_node& node) = 0;
            virtual void visit(const conditional_node& node) = 0;
            virtual void visit(const while_node& node) = 0;
        };
    }
}
