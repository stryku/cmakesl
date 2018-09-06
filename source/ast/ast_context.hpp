#pragma once

#include "ast/type.hpp"
#include "ast/function.hpp"
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
            explicit ast_context(const ast_context* parent = nullptr);
            virtual ~ast_context() = default;

            void add_type(std::unique_ptr<type> t);
            const type* find_type(cmsl::string_view name) const;

            void add_function(std::unique_ptr<function> fun);
            const function* find_function(cmsl::string_view name) const;

        private:
            const ast_context* m_parent;
            std::vector<std::unique_ptr<type>> m_types;
            std::vector<std::unique_ptr<function>> m_functions;
        };
    }
}
