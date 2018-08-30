#pragma once

#include "ast/type_kind.hpp"
#include "common/enum_class_utils.hpp"

#include <unordered_set>
#include <vector>

namespace cmsl
{
    namespace ast
    {
        class type_conversion_check
        {
        private:
            using conversion_group_t = enum_unordered_set<type_kind>;
            using conversion_group_ref_t = std::reference_wrapper<const conversion_group_t>;
            using conversion_map_t = enum_unordered_map<type_kind, conversion_group_ref_t>;

        public:
            explicit type_conversion_check();

            bool can_convert(type_kind from, type_kind to) const;

        private:
            std::vector<conversion_group_t> get_conversion_groups() const;
            conversion_map_t get_conversion_map() const;
            const conversion_group_t* get_group_for_type(type_kind kind) const;

        private:
            std::vector<conversion_group_t> m_conversion_groups;
            conversion_map_t m_conversion_map;
        };
    }
}


