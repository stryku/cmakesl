#pragma once

#include "common/string.hpp"

#include <unordered_map>

namespace cmsl
{
    namespace exec
    {
        class scope_context
        {
        public:
            explicit scope_context(const scope_context* parent);

            void add_variable(cmsl::string_view name, int value);
            int* get_variable(cmsl::string_view name);

            bool variable_exists(cmsl::string_view name) const;

        private:
            string_view_map<int> m_variables;
            const scope_context* m_parent;
        };
    }
}
