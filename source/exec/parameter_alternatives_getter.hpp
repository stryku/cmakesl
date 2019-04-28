#pragma once

#include "exec/instance/instance_value_alternative.hpp"
#include "exec/instance/int_alias.hpp"
#include "exec/instance/version_value.hpp"
#include "exec/instance/list_value.hpp"
#include "exec/instance/instance.hpp"

#include <tuple>

namespace cmsl::exec
{

    using index_t = inst::instance_value_alternative;

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

            // get_param() and its specializations have to be outside of the class.
            template <index_t Alternative>
            decltype(auto) get_param(const inst::instance& instance);

            template <>
            decltype(auto) get_param<index_t::bool_>(const inst::instance& instance)
            {
                return instance.value_cref().get_bool();
            }

            template <>
            decltype(auto) get_param<index_t::int_>(const inst::instance& instance)
            {
                return instance.value_cref().get_int();
            }

            template <>
            decltype(auto) get_param<index_t::double_>(const inst::instance& instance)
            {
                return instance.value_cref().get_double();
            }

            template <>
            decltype(auto) get_param<index_t::string>(const inst::instance& instance)
            {
                return instance.value_cref().get_string_cref();
            }

            template <>
            decltype(auto) get_param<index_t::version>(const inst::instance& instance)
            {
                return instance.value_cref().get_version_cref();
            }

            template <>
            decltype(auto) get_param<index_t::list>(const inst::instance& instance)
            {
                return instance.value_cref().get_list_cref();
            }

            template <>
            decltype(auto) get_param<index_t::library>(const inst::instance& instance)
            {
                return instance.value_cref().get_library_cref();
            }

            template <>
            decltype(auto) get_param<index_t::executable>(const inst::instance& instance)
            {
                return instance.value_cref().get_executable_cref();
            }
        }

        template <inst::instance_value_alternative... Alternatives>
        class parameter_alternatives_getter
        {
        private:

        public:
            template <typename Params>
            static auto get(Params&& params)
            {
                return get_impl(std::forward<Params>(params),
                           std::make_index_sequence<sizeof...(Alternatives)>{});
            }

        private:
            template <typename Params, auto... Is>
            static decltype(auto) get_impl(Params&& params, std::index_sequence<Is...>)
            {
                static constexpr index_t alternatives[] = { Alternatives... };
                return std::make_tuple(details::get_param<alternatives[Is]>(*params[Is])...);
            }
        };
}
