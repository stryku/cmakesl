#pragma once

namespace cmsl
{
    namespace sema
    {
        class sema_tree_visitor;

        class sema_node
        {
        public:
            virtual ~sema_node() = default;

            virtual void visit(sema_tree_visitor& visitor) = 0;
        };
    }
}
