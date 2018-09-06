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
    constexpr auto is_one_of_v = is_one_of<T, Ts...>::value;

    template <typename T, typename... Ts>
    using is_not_one_of = details::disjunction<details::negation<std::is_same<T, Ts>>...>;

    template <typename T>
    struct is_list : std::false_type
    {};

    template <typename V>
    struct is_list<std::list<V>> : std::true_type
    {};

    template <typename T>
    constexpr auto is_list_v = is_list<T>::value;

    template <typename T>
    using list_mock_t = std::conditional_t<is_list_v<T>, T, std::false_type>;

    template <typename T>
    constexpr auto is_container_v = is_one_of<std::decay_t<T>, std::string, list_mock_t<T>>::value;

    template <typename T>
    using enable_if_container = std::enable_if_t<is_container_v<T>>;

    template <typename T>
    using enable_if_not_container = std::enable_if_t<!is_container_v<T>>;

    template <typename T>
    using enable_if_has_not_size = std::enable_if_t<!is_container_v<T>>;
}
