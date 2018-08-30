#include "ast/type_conversion_check.hpp"
#include "ast/type_kind.hpp"
#include "common/algorithm.hpp"

namespace cmsl
{
    namespace ast
    {
        type_conversion_check::type_conversion_check()
            : m_conversion_groups{ get_conversion_groups() }
            , m_conversion_map{ get_conversion_map() }
        {}

        std::vector<type_conversion_check::conversion_group_t> type_conversion_check::get_conversion_groups() const
        {
            return {
                conversion_group_t{ type_kind::k_bool, type_kind ::k_int, type_kind::k_double}
            };
        }

        type_conversion_check::conversion_map_t type_conversion_check::get_conversion_map() const
        {
            conversion_map_t map;

            for(const auto& group : m_conversion_groups)
            {
                for(const auto kind : group)
                {
                    map.emplace(kind, conversion_group_ref_t{ group });
                }
            }

            return map;
        }

        const type_conversion_check::conversion_group_t* type_conversion_check::get_group_for_type(type_kind kind) const
        {
            const auto found = m_conversion_map.find(kind);

            if(found == std::cend(m_conversion_map))
            {
                return nullptr;
            }

            return &found->second.get();
        }


        bool type_conversion_check::can_convert(type_kind from, type_kind to) const
        {
            const auto group = get_group_for_type(from);

            if(group == nullptr)
            {
                return false;
            }

            return contains(*group, to);
        }
    }
}
