#pragma once

namespace cmsl
{
    namespace ast
    {
        class ast_context
        {
        public:
            explicit ast_context(ast_context* parent);

        private:
            ast_context* m_parent;
        };
    }
}
