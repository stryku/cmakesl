#pragma once

#include "exec/stmt/statement.hpp"

namespace cmsl
{
    namespace ast
    {
        class while_node;
    }

    namespace exec
    {
        namespace stmt
        {
            class while_statement : public statement
            {
            public:
                explicit while_statement(const ast::while_node& node);

                virtual void execute(execution& e) override;

            private:
                bool condition_passed(execution &e);

            private:
                const ast::while_node& m_node;
            };
        }
    }
}
