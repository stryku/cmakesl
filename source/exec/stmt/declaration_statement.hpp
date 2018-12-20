#pragma once

#include "exec/stmt/statement.hpp"

namespace cmsl
{
    namespace ast
    {
        class declaration_node;
    }

    namespace exec
    {
        namespace stmt
        {
            class declaration_statement : public statement
            {
            public:
                explicit declaration_statement(const ast::declaration_node& node);

                virtual void execute(execution& e) override;

            private:
                const ast::declaration_node& m_node;
            };
        }
    }
}
