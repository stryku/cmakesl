#pragma once

#include "ast/type_kind.hpp"
#include "ast/type_conversion_checker.hpp"
#include "common/enum_class_utils.hpp"

namespace cmsl
{
    namespace ast
    {
        class type;

        class common_type_finder
        {
        private:
            using type_ranks_t = enum_unordered_map<type_kind, size_t>;

        public:
            explicit common_type_finder();

            // Returns nullptr when there is no common type
            const type* find_common_type(const type& t1, const type& t2) const;

        private:
            type_ranks_t get_type_ranks() const;

        private:
            type_ranks_t m_type_ranks;
            type_conversion_checker m_conversion_check;
        };
    }
}


