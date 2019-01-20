#pragma once

namespace cmsl
{
    namespace sema
    {
        class sema_node_visitor;

        class sema_node
        {
        public:
            virtual ~sema_node() = default;

            virtual void visit(sema_node_visitor& visitor) = 0;
        };
    }
}
