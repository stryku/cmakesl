#include "ast/type_kind.hpp"

#include <algorithm>

namespace cmsl
{
    namespace ast
    {
        type_kind type_kind_from_string(cmsl::string_view str)
        {
            const auto map = {
                std::make_pair("int", type_kind::k_int),
                std::make_pair("double_", type_kind::k_double)
            };

            const auto it = std::find_if(std::cbegin(map), std::cend(map),
                                         [str](const auto pair)
                                         {
                                             return pair.first == str;
                                         });

            if (it != std::cend(map))
            {
                return it->second;
            }

            return type_kind::k_user;
        }
    }
}
