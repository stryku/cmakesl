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

                virtual void execute(source_executor& e) override;

            private:
                bool condition_passed(source_executor &e);

            private:
                const ast::while_node& m_node;
            };
        }
    }
}
