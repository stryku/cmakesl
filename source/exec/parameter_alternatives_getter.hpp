#pragma once

#include "exec/instance/instance_value_alternative.hpp"
#include "exec/instance/int_alias.hpp"
#include "exec/instance/version_value.hpp"
#include "exec/instance/list_value.hpp"
#include "exec/instance/instance.hpp"

#include <tuple>

namespace cmsl
{
    namespace exec
    {
        namespace details
        {
            using alternatives_tuple_t = std::tuple<bool,
                                                    inst::int_t,
                                                    double,
                                                    std::string,
                                                    inst::version_value,
                                                    inst::list_value>;


            template <inst::instance_value_alternative Alternative>
            using alternative_type_t = decltype(std::get<Alternative>(std::declval<const alternatives_tuple_t>()));
        }

        template <inst::instance_value_alternative... Alternatives>
        class parameter_alternatives_getter
        {
        public:
            template <typename Params>
            static auto get(Params&& params)
            {
                return get(std::forward<Params>(params),
                           std::make_index_sequence<sizeof...(Alternatives)>{});
            }

        private:
            using index_t = inst::instance_value_alternative;

            template <typename Params, auto... Is>
            static decltype(auto) get(Params&& params, std::index_sequence<Is...>)
            {
                static constexpr index_t alternatives[] = { Alternatives... };
                return std::make_tuple(get<alternatives[Is]>(params[Is])...);
            }

            template <index_t Alternative>
            decltype(auto) get(const inst::instance& instance);

            template <>
            decltype(auto) get<index_t::bool_>(const inst::instance& instance)
            {
                return instance.get_value_cref().get_bool();
            }

            template <>
            decltype(auto) get<index_t::int_>(const inst::instance& instance)
            {
                return instance.get_value_cref().get_int();
            }

            template <>
            decltype(auto) get<index_t::double_>(const inst::instance& instance)
            {
                return instance.get_value_cref().get_double();
            }

            template <>
            decltype(auto) get<index_t::string>(const inst::instance& instance)
            {
                return instance.get_value_cref().get_string_cref();
            }

            template <>
            decltype(auto) get<index_t::version>(const inst::instance& instance)
            {
                return instance.get_value_cref().get_version_cref();
            }

            template <>
            decltype(auto) get<index_t::list>(const inst::instance& instance)
            {
                return instance.get_value_cref().get_list_cref();
            }
        };
    }
}
