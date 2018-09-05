#include "ast/common_type_finder.hpp"
#include "ast/type.hpp"

namespace cmsl
{
    namespace ast
    {
        common_type_finder::common_type_finder()
            : m_type_ranks { get_type_ranks() }
        {}

        const type* common_type_finder::find_common_type(const type &t1, const type &t2) const
        {
            const auto kind1 = t1.get_kind();
            const auto kind2 = t2.get_kind();

            if(t1 == t2)
            {
                // Very same user types, return any of them
                return &t1;
            }

            if(t1.is_user() || t2.is_user())
            {
                // Possible combinations:
                // user user
                // user builtin
                // builtin user
                // In any of these cases conversion is not possible => there is no common type
                return nullptr;
            }
            else
            {
                // At this point we know that t1 and t2 are builtin types

                const auto rank1 = m_type_ranks.at(kind1);
                const auto rank2 = m_type_ranks.at(kind2);

                if(m_conversion_check.can_convert(kind2, kind1) && rank1 >= rank2)
                {
                    return &t1;
                }
                else if(m_conversion_check.can_convert(kind1, kind2) && rank2 >= rank1)
                {
                    return &t2;
                }
            }

            return nullptr;
        }

        common_type_finder::type_ranks_t common_type_finder::get_type_ranks() const
        {
            return type_ranks_t {
                { type_kind::k_bool, 1 },
                { type_kind::k_int, 2 },
                { type_kind::k_double, 3 }
            };
        }
    }
}


