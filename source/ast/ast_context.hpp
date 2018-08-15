#pragma once

#include "ast/type.hpp"
#include "ast/function_node.hpp"
#include "common/string.hpp"

#include <memory>
#include <vector>

namespace cmsl
{
    namespace ast
    {
        class ast_context
        {
        public:
            explicit ast_context(ast_context* parent = nullptr);
            virtual ~ast_context() {}

            void add_type(std::unique_ptr<type> t);
            const type* find_type(cmsl::string_view name) const;

            void add_function(std::unique_ptr<function> fun);
            const function* find_function(cmsl::string_view name) const;

        private:
            ast_context* m_parent;
            std::vector<std::unique_ptr<type>> m_types;
            std::vector<std::unique_ptr<function>> m_functions;
        };
    }
}
