#pragma once

namespace cmsl
{
    namespace sema
    {
        class variable_declaration_node;
        class int_value_node;

        class sema_node_visitor
        {
        public:
            virtual ~sema_node_visitor() = default;

            virtual void visit(const variable_declaration_node& node) = 0;
            virtual void visit(const int_value_node& node) = 0;
        };
    }
}
