#pragma once

#include "common/string.hpp"

#include <boost/functional/hash.hpp>

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
            struct hasher
            {
                auto operator()(cmsl::string_view str) const
                {
                    return boost::hash_range(str.begin(), str.end());
                }
            };

            std::unordered_map<cmsl::string_view, int, hasher> m_variables;

            const scope_context* m_parent;
        };
    }
}
