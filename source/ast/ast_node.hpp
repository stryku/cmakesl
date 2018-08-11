#pragma once

#include "ast/expression_type.hpp"

namespace cmsl
{
    namespace exec
    {
        class executor;
    }

    namespace ast
    {
        class ast_node
        {
        public:
            ast_node(expression_type type)
                : m_type{ type }
            {}

            virtual ~ast_node()
            {}

            virtual void execute(exec::executor& executor) {};

            expression_type get_type() const
            {
                return m_type;
            }

        private:
            expression_type m_type;
        };
    }
}
