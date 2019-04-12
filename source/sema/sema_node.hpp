#pragma once

namespace cmsl
{
    struct source_location;

    namespace ast
    {
        class ast_node;
    }

    namespace sema
    {
        class sema_node_visitor;

        class sema_node
        {
        public:
            explicit sema_node(const ast::ast_node& ast_node);

            virtual ~sema_node() = default;

            virtual void visit(sema_node_visitor &visitor) const = 0;
            source_location begin_location() const;
            source_location end_location() const;
            const ast::ast_node& ast_node() const;

        private:
            const ast::ast_node& m_ast_node;
        };
    }
}
