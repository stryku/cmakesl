#pragma once

namespace cmsl
{
    namespace ast
    {
        class block_node;
        class class_node2;
        class conditional_node;
        class if_else_node;
        class binary_operator_node;
        class class_member_access_node;
        class function_call_node;
        class member_function_call_node;
        class bool_value_node;
        class int_value_node;
        class double_value_node;
        class string_value_node;
        class id_node;
        class return_node;
        class translation_unit_node;
        class user_function_node2;
        class variable_declaration_node;
        class while_node;

        class ast_node_visitor
        {
        public:
            virtual ~ast_node_visitor() = default;

            virtual void visit(const block_node& node) = 0;
            virtual void visit(const class_node2& node) = 0;
            virtual void visit(const conditional_node& node) = 0;
            virtual void visit(const if_else_node& node) = 0;
            virtual void visit(const binary_operator_node& node) = 0;
            virtual void visit(const class_member_access_node& node) = 0;
            virtual void visit(const function_call_node& node) = 0;
            virtual void visit(const member_function_call_node& node) = 0;
            virtual void visit(const bool_value_node& node) = 0;
            virtual void visit(const int_value_node& node) = 0;
            virtual void visit(const double_value_node& node) = 0;
            virtual void visit(const string_value_node& node) = 0;
            virtual void visit(const id_node& node) = 0;
            virtual void visit(const return_node& node) = 0;
            virtual void visit(const translation_unit_node& node) = 0;
            virtual void visit(const user_function_node2& node) = 0;
            virtual void visit(const variable_declaration_node& node) = 0;
            virtual void visit(const while_node& node) = 0;
        };

        class empty_ast_node_visitor : public ast_node_visitor
        {
        public:
            virtual ~empty_ast_node_visitor() = default;

            virtual void visit(const block_node& node) override {}
            virtual void visit(const class_node2& node) override {}
            virtual void visit(const if_else_node& node) = 0;
            virtual void visit(const conditional_node& node) override {}
            virtual void visit(const binary_operator_node& node) override {}
            virtual void visit(const class_member_access_node& node) override {}
            virtual void visit(const function_call_node& node) override {}
            virtual void visit(const member_function_call_node& node) override {}
            virtual void visit(const int_value_node& node) override {}
            virtual void visit(const bool_value_node& node) override {}
            virtual void visit(const double_value_node& node) override {}
            virtual void visit(const string_value_node& node) override {}
            virtual void visit(const id_node& node) override {}
            virtual void visit(const return_node& node) override {}
            virtual void visit(const translation_unit_node& node) override {}
            virtual void visit(const user_function_node2& node) override {}
            virtual void visit(const variable_declaration_node& node) override {}
            virtual void visit(const while_node& node) override {}
        };
    }
}
