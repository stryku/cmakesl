#pragma once

#include "exec/stmt/statement.hpp"

namespace cmsl
{
    namespace ast
    {
        class return_node;
    }

    namespace exec
    {
        namespace stmt
        {
            class return_statement : public statement
            {
            public:
                explicit return_statement(ast::return_node& node);

                virtual void execute(source_executor& e) override;

            private:
                ast::return_node& m_node;
            };
        }
    }
}
