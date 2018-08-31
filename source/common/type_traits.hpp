#pragma once

#include <type_traits>

namespace cmsl
{
    namespace details
    {
        // todo use std::disjunction
        template<class...> struct disjunction : std::false_type { };
        template<class B1> struct disjunction<B1> : B1 { };
        template<class B1, class... Bn>
        struct disjunction<B1, Bn...>
                : std::conditional_t<bool(B1::value), B1, disjunction<Bn...>>  { };

    }

    template <typename... Ts>
    using is_one_of = details::disjunction<std::is_same_v<Ts>...>;
}
