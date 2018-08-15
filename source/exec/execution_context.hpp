#pragma once

#include "exec/scope_context.hpp"

#include <stack>

namespace cmsl
{
    namespace exec
    {
        class execution_context
        {
        public:
            void add_variable(cmsl::string_view name, int value);
            int* get_variable(cmsl::string_view name);

            bool variable_exists(cmsl::string_view name) const;

            void enter_scope();
            void leave_scope();

        private:
            scope_context* current_scope();
            const scope_context* current_scope() const;

        private:
            std::stack<scope_context> m_scopes;
        };
    }
}
