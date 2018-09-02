#pragma once

#include "exec/stmt/statement.hpp"

namespace cmsl
{
    namespace ast
    {
        class if_else_node;
    }

    namespace exec
    {
        namespace stmt
        {
            class if_else_statement : public statement
            {
            public:
                explicit if_else_statement(const ast::if_else_node& node);

                virtual void execute(execution& e) override;

            private:
                const ast::if_else_node& m_node;
            };
        }
    }
}


