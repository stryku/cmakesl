#pragma once

#include "common/enum_class_map.hpp"

#include <unordered_set>
#include <vector>

namespace cmsl
{
    namespace ast
    {
        enum class type_kind;
        class type;

        class type_conversion_checker
        {
        private:
            using conversion_group_t = std::unordered_set<type_kind>;
            using conversion_map_t = enum_unordered_map<type_kind, std::reference_wrapper<const conversion_group_t>>;

        public:
            explicit type_conversion_checker();

            bool can_convert(const type& from, const type& to) const;

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


