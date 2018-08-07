#pragma once

#include "common/string.hpp"

#include <memory>
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
            virtual ~ast_context() {}

            void add_type(std::unique_ptr<type> t);
            type* find_type(cmsl::string_view name);

        private:
            ast_context* m_parent;
            std::vector<std::unique_ptr<type>> m_types;
        };
    }
}
