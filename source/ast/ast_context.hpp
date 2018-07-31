#pragma once

#include "common/string.hpp"

#include <vector>

namespace cmsl
{
    namespace ast
    {
        class type;

        class ast_context
        {
        public:
            explicit ast_context(ast_context* parent = nullptr);

            void add_type(type* t);
            type* find_type(cmsl::string_view name);

        private:
            ast_context* m_parent;
            std::vector<type*> m_types;
        };
    }
}
