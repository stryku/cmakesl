#pragma once

#include <type_traits>

namespace cmsl
{
    namespace details
    {
        // todo use std stuff
        template<class...> struct disjunction : std::false_type { };
        template<class B1> struct disjunction<B1> : B1 { };
        template<class B1, class... Bn>
        struct disjunction<B1, Bn...>
                : std::conditional_t<bool(B1::value), B1, disjunction<Bn...>>  { };

        template<class...> struct conjunction : std::true_type { };
        template<class B1> struct conjunction<B1> : B1 { };
        template<class B1, class... Bn>
        struct conjunction<B1, Bn...>
                : std::conditional_t<bool(B1::value), conjunction<Bn...>, B1> {};

        template <bool B>
        using bool_constant = std::integral_constant<bool, B>;

        template<class B>
        struct negation : bool_constant<!bool(B::value)> { };
    }

    template <typename T, typename... Ts>
    using is_one_of = details::disjunction<std::is_same<T, Ts>...>;

    template <typename T, typename... Ts>
    using is_not_one_of = details::disjunction<details::negation<std::is_same<T, Ts>>...>;

    template <typename T>
    constexpr auto is_container_v = is_one_of<std::decay_t<T>, std::string>::value;

    template <typename T>
    using enable_if_container = std::enable_if_t<is_container_v<T>>;

    template <typename T>
    using enable_if_not_container = std::enable_if_t<!is_container_v<T>>;
}
