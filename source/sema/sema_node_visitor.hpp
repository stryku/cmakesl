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
        };
    }
}
