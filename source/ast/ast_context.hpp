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
        // todo: this interface exists only for tests purposes. Remove it and substitute implementation in tests with the mocked one
        class ast_context
        {
        public:
            virtual ~ast_context() = default;

            virtual void add_type(std::unique_ptr<type> t) = 0;
            virtual const type* find_type(cmsl::string_view name) const = 0;

            virtual void add_function(std::unique_ptr<function> fun) = 0;
            virtual const function* find_function(cmsl::string_view name) const = 0;
        };

        class ast_context_impl : public ast_context
        {
        public:
            explicit ast_context_impl(const ast_context* parent = nullptr);
            virtual ~ast_context_impl() = default;

            void add_type(std::unique_ptr<type> t) override;
            const type* find_type(cmsl::string_view name) const override;

            void add_function(std::unique_ptr<function> fun) override;
            const function* find_function(cmsl::string_view name) const override;

        private:
            const ast_context* m_parent;
            std::vector<std::unique_ptr<type>> m_types;
            std::vector<std::unique_ptr<function>> m_functions;
        };
    }
}
