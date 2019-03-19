#include "exec/builtin_function_caller2.hpp"
#include "sema/builtin_function_kind.hpp"
#include "exec/instance/instance.hpp"
#include "common/assert.hpp"
#include "builtin_function_caller2.hpp"
#include "exec/instance/instances_holder.hpp"
#include "exec/parameter_alternatives_getter.hpp"

#include <algorithm>
#include <sema/homogeneous_generic_type.hpp>

#define CASE_BUILTIN_FUNCTION_CALL(function) \
case sema::builtin_function_kind::function: \
{ \
    result = function(instance, params); \
} break

namespace cmsl
{
    namespace exec
    {
        template <auto... Alternatives, typename Params>
        decltype(auto) get_params(Params&& params)
        {
            using params_getter_t = parameter_alternatives_getter<Alternatives...>;
            return params_getter_t::get(std::forward<Params>(params));
        }

        using alternative_t = inst::instance_value_alternative;

        const std::locale builtin_function_caller2::m_utf8_locale("en_US.utf8");

        builtin_function_caller2::builtin_function_caller2(inst::instances_holder_interface &instances)
            : m_instances{ instances }
        {}

        std::unique_ptr<inst::instance>
        builtin_function_caller2::call_member(inst::instance &instance, sema::builtin_function_kind function_kind, const params_t &params)
        {
            inst::instance* result{ nullptr };

            switch (function_kind)
            {
                // bool
                CASE_BUILTIN_FUNCTION_CALL(bool_ctor);
                CASE_BUILTIN_FUNCTION_CALL(bool_ctor_bool);
                CASE_BUILTIN_FUNCTION_CALL(bool_ctor_int);
                CASE_BUILTIN_FUNCTION_CALL(bool_operator_equal);
                CASE_BUILTIN_FUNCTION_CALL(bool_operator_equal_equal);
                CASE_BUILTIN_FUNCTION_CALL(bool_operator_pipe_pipe);
                CASE_BUILTIN_FUNCTION_CALL(bool_operator_amp_amp);
                CASE_BUILTIN_FUNCTION_CALL(bool_to_string);

                // int
                CASE_BUILTIN_FUNCTION_CALL(int_ctor);
                CASE_BUILTIN_FUNCTION_CALL(int_ctor_bool);
                CASE_BUILTIN_FUNCTION_CALL(int_ctor_int);
                CASE_BUILTIN_FUNCTION_CALL(int_ctor_double);
                CASE_BUILTIN_FUNCTION_CALL(int_operator_plus);
                CASE_BUILTIN_FUNCTION_CALL(int_operator_minus);
                CASE_BUILTIN_FUNCTION_CALL(int_operator_star);
                CASE_BUILTIN_FUNCTION_CALL(int_operator_slash);
                CASE_BUILTIN_FUNCTION_CALL(int_operator_equal);
                CASE_BUILTIN_FUNCTION_CALL(int_operator_plus_equal);
                CASE_BUILTIN_FUNCTION_CALL(int_operator_minus_equal);
                CASE_BUILTIN_FUNCTION_CALL(int_operator_star_equal);
                CASE_BUILTIN_FUNCTION_CALL(int_operator_slash_equal);
                CASE_BUILTIN_FUNCTION_CALL(int_operator_less);
                CASE_BUILTIN_FUNCTION_CALL(int_operator_less_equal);
                CASE_BUILTIN_FUNCTION_CALL(int_operator_greater);
                CASE_BUILTIN_FUNCTION_CALL(int_operator_greater_equal);
                CASE_BUILTIN_FUNCTION_CALL(int_operator_equal_equal);
                CASE_BUILTIN_FUNCTION_CALL(int_to_string);

                // double
                CASE_BUILTIN_FUNCTION_CALL(double_ctor);
                CASE_BUILTIN_FUNCTION_CALL(double_ctor_double);
                CASE_BUILTIN_FUNCTION_CALL(double_ctor_int);
                CASE_BUILTIN_FUNCTION_CALL(double_operator_plus);
                CASE_BUILTIN_FUNCTION_CALL(double_operator_minus);
                CASE_BUILTIN_FUNCTION_CALL(double_operator_star);
                CASE_BUILTIN_FUNCTION_CALL(double_operator_slash);
                CASE_BUILTIN_FUNCTION_CALL(double_operator_equal);
                CASE_BUILTIN_FUNCTION_CALL(double_operator_plus_equal);
                CASE_BUILTIN_FUNCTION_CALL(double_operator_minus_equal);
                CASE_BUILTIN_FUNCTION_CALL(double_operator_star_equal);
                CASE_BUILTIN_FUNCTION_CALL(double_operator_slash_equal);
                CASE_BUILTIN_FUNCTION_CALL(double_operator_less);
                CASE_BUILTIN_FUNCTION_CALL(double_operator_less_equal);
                CASE_BUILTIN_FUNCTION_CALL(double_operator_greater);
                CASE_BUILTIN_FUNCTION_CALL(double_operator_greater_equal);

                // string
                CASE_BUILTIN_FUNCTION_CALL(string_ctor);
                CASE_BUILTIN_FUNCTION_CALL(string_ctor_string);
                CASE_BUILTIN_FUNCTION_CALL(string_ctor_string_count);
                CASE_BUILTIN_FUNCTION_CALL(string_empty);
                CASE_BUILTIN_FUNCTION_CALL(string_size);
                CASE_BUILTIN_FUNCTION_CALL(string_operator_equal_equal);
                CASE_BUILTIN_FUNCTION_CALL(string_operator_not_equal);
                CASE_BUILTIN_FUNCTION_CALL(string_operator_less);
                CASE_BUILTIN_FUNCTION_CALL(string_operator_less_equal);
                CASE_BUILTIN_FUNCTION_CALL(string_operator_greater);
                CASE_BUILTIN_FUNCTION_CALL(string_operator_greater_equal);
                CASE_BUILTIN_FUNCTION_CALL(string_operator_plus);
                CASE_BUILTIN_FUNCTION_CALL(string_operator_plus_equal);
                CASE_BUILTIN_FUNCTION_CALL(string_clear);
                CASE_BUILTIN_FUNCTION_CALL(string_insert_pos_str);
                CASE_BUILTIN_FUNCTION_CALL(string_erase_pos);
                CASE_BUILTIN_FUNCTION_CALL(string_erase_pos_count);
                CASE_BUILTIN_FUNCTION_CALL(string_starts_with);
                CASE_BUILTIN_FUNCTION_CALL(string_ends_with);
                CASE_BUILTIN_FUNCTION_CALL(string_replace_pos_count_str);
                CASE_BUILTIN_FUNCTION_CALL(string_substr_pos);
                CASE_BUILTIN_FUNCTION_CALL(string_substr_pos_count);
                CASE_BUILTIN_FUNCTION_CALL(string_resize_newsize);
                CASE_BUILTIN_FUNCTION_CALL(string_resize_newsize_fill);
                CASE_BUILTIN_FUNCTION_CALL(string_find_str);
                CASE_BUILTIN_FUNCTION_CALL(string_find_str_pos);
                CASE_BUILTIN_FUNCTION_CALL(string_find_not_of_str);
                CASE_BUILTIN_FUNCTION_CALL(string_find_not_of_str_pos);
                CASE_BUILTIN_FUNCTION_CALL(string_find_last_str);
                CASE_BUILTIN_FUNCTION_CALL(string_find_last_not_of_str);
                CASE_BUILTIN_FUNCTION_CALL(string_contains);
                CASE_BUILTIN_FUNCTION_CALL(string_lower);
                CASE_BUILTIN_FUNCTION_CALL(string_make_lower);
                CASE_BUILTIN_FUNCTION_CALL(string_upper);
                CASE_BUILTIN_FUNCTION_CALL(string_make_upper);

                // version
                CASE_BUILTIN_FUNCTION_CALL(version_ctor_major);
                CASE_BUILTIN_FUNCTION_CALL(version_ctor_major_minor);
                CASE_BUILTIN_FUNCTION_CALL(version_ctor_major_minor_patch);
                CASE_BUILTIN_FUNCTION_CALL(version_ctor_major_minor_patch_tweak);
                CASE_BUILTIN_FUNCTION_CALL(version_operator_equal_equal);
                CASE_BUILTIN_FUNCTION_CALL(version_operator_not_equal);
                CASE_BUILTIN_FUNCTION_CALL(version_operator_less);
                CASE_BUILTIN_FUNCTION_CALL(version_operator_less_equal);
                CASE_BUILTIN_FUNCTION_CALL(version_operator_greater);
                CASE_BUILTIN_FUNCTION_CALL(version_operator_greater_equal);
                CASE_BUILTIN_FUNCTION_CALL(version_major);
                CASE_BUILTIN_FUNCTION_CALL(version_minor);
                CASE_BUILTIN_FUNCTION_CALL(version_patch);
                CASE_BUILTIN_FUNCTION_CALL(version_tweak);
                CASE_BUILTIN_FUNCTION_CALL(version_to_string);

                // list
                CASE_BUILTIN_FUNCTION_CALL(list_ctor);
                CASE_BUILTIN_FUNCTION_CALL(list_ctor_list);
                CASE_BUILTIN_FUNCTION_CALL(list_ctor_value);
                CASE_BUILTIN_FUNCTION_CALL(list_ctor_value_count);
                CASE_BUILTIN_FUNCTION_CALL(list_push_back_value);
                CASE_BUILTIN_FUNCTION_CALL(list_push_back_list);
                CASE_BUILTIN_FUNCTION_CALL(list_push_front_value);
                CASE_BUILTIN_FUNCTION_CALL(list_push_front_list);
                CASE_BUILTIN_FUNCTION_CALL(list_pop_back);
                CASE_BUILTIN_FUNCTION_CALL(list_pop_front);
                CASE_BUILTIN_FUNCTION_CALL(list_at);
                CASE_BUILTIN_FUNCTION_CALL(list_front);
                CASE_BUILTIN_FUNCTION_CALL(list_back);
                CASE_BUILTIN_FUNCTION_CALL(list_insert_pos_value);
                CASE_BUILTIN_FUNCTION_CALL(list_insert_pos_list);
                CASE_BUILTIN_FUNCTION_CALL(list_erase_pos);
                CASE_BUILTIN_FUNCTION_CALL(list_erase_pos_count);
                CASE_BUILTIN_FUNCTION_CALL(list_remove_value);
                CASE_BUILTIN_FUNCTION_CALL(list_remove_value_count);
                CASE_BUILTIN_FUNCTION_CALL(list_remove_last_value_count);
                CASE_BUILTIN_FUNCTION_CALL(list_clear);
                CASE_BUILTIN_FUNCTION_CALL(list_resize);
                CASE_BUILTIN_FUNCTION_CALL(list_sort);
                CASE_BUILTIN_FUNCTION_CALL(list_reverse);
                CASE_BUILTIN_FUNCTION_CALL(list_min);
                CASE_BUILTIN_FUNCTION_CALL(list_max);
                CASE_BUILTIN_FUNCTION_CALL(list_sublist_pos);
                CASE_BUILTIN_FUNCTION_CALL(list_sublist_pos_count);
                CASE_BUILTIN_FUNCTION_CALL(list_size);
                CASE_BUILTIN_FUNCTION_CALL(list_empty);
                CASE_BUILTIN_FUNCTION_CALL(list_find_value);
                CASE_BUILTIN_FUNCTION_CALL(list_find_value_pos);
                CASE_BUILTIN_FUNCTION_CALL(list_operator_plus_value);
                CASE_BUILTIN_FUNCTION_CALL(list_operator_plus_list);
                CASE_BUILTIN_FUNCTION_CALL(list_operator_plus_equal_value);
                CASE_BUILTIN_FUNCTION_CALL(list_operator_plus_equal_list);

                default:
                    CMSL_UNREACHABLE("Calling unimplemented member function");
                    return nullptr;

            }

            return m_instances.gather_ownership(result);
        }

        inst::instance*
        builtin_function_caller2::int_operator_minus(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            const auto lhs = instance.get_value_cref()
                                     .get_int();
            const auto& [rhs] = get_params<alternative_t::int_>(params);
            return m_instances.create2(lhs - rhs);
        }

        inst::instance*
        builtin_function_caller2::int_operator_equal(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            const auto& [rhs] = get_params<alternative_t::int_>(params);
            instance.get_value_ref() = rhs;
            return m_instances.create2_reference(instance);
        }

        inst::instance *
        builtin_function_caller2::int_ctor_bool(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            const auto& [param] = get_params<alternative_t::bool_>(params);
            instance.get_value_ref().set_int(static_cast<inst::int_t>(param));
            return &instance;
        }

        inst::instance *
        builtin_function_caller2::bool_ctor(inst::instance &instance, const builtin_function_caller2::params_t&)
        {
            instance.get_value_ref().set_bool(false);
            return &instance;
        }

        inst::instance *
        builtin_function_caller2::bool_ctor_bool(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            const auto& [param] = get_params<alternative_t::bool_>(params);
            instance.get_value_ref().set_bool(param);
            return &instance;
        }

        inst::instance *
        builtin_function_caller2::bool_ctor_int(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            const auto& [param] = get_params<alternative_t::int_>(params);
            instance.get_value_ref().set_bool(static_cast<bool>(param));
            return &instance;
        }

        inst::instance *
        builtin_function_caller2::bool_operator_equal(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            const auto& [rhs] = get_params<alternative_t::bool_>(params);
            instance.get_value_ref() = rhs;
            return m_instances.create2_reference(instance);
        }

        inst::instance *
        builtin_function_caller2::int_ctor(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            instance.get_value_ref().set_int(inst::int_t{ 0 });
            return &instance;
        }

        inst::instance *
        builtin_function_caller2::int_ctor_int(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            const auto& [rhs] = get_params<alternative_t::int_>(params);
            instance.get_value_ref() = rhs;
            return &instance;
        }

        inst::instance *
        builtin_function_caller2::int_operator_plus(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            const auto lhs = instance.get_value_cref()
                                     .get_int();
            const auto& [rhs] = get_params<alternative_t::int_>(params);
            return m_instances.create2(lhs + rhs);
        }

        inst::instance *
        builtin_function_caller2::int_operator_plus_equal(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            const auto lhs = instance.get_value_cref()
                                     .get_int();
            const auto& [rhs] = get_params<alternative_t::int_>(params);

            instance.get_value_ref().set_int(lhs + rhs);
            return m_instances.create2_reference(instance);
        }

        inst::instance *
        builtin_function_caller2::int_operator_minus_equal(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            const auto lhs = instance.get_value_cref()
                                     .get_int();
            const auto& [rhs] = get_params<alternative_t::int_>(params);

            instance.get_value_ref().set_int(lhs - rhs);
            return m_instances.create2_reference(instance);
        }

        inst::instance *
        builtin_function_caller2::int_operator_star(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            const auto lhs = instance.get_value_cref()
                                     .get_int();
            const auto& [rhs] = get_params<alternative_t::int_>(params);
            return m_instances.create2(lhs * rhs);
        }

        inst::instance *
        builtin_function_caller2::int_operator_slash(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            const auto lhs = instance.get_value_cref()
                                     .get_int();
            const auto& [rhs] = get_params<alternative_t::int_>(params);
            return m_instances.create2(lhs / rhs);
        }

        inst::instance *
        builtin_function_caller2::int_operator_star_equal(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            const auto lhs = instance.get_value_cref()
                                     .get_int();
            const auto& [rhs] = get_params<alternative_t::int_>(params);

            instance.get_value_ref().set_int(lhs * rhs);
            return m_instances.create2_reference(instance);
        }

        inst::instance *
        builtin_function_caller2::int_operator_slash_equal(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            const auto lhs = instance.get_value_cref()
                                     .get_int();
            const auto& [rhs] = get_params<alternative_t::int_>(params);

            instance.get_value_ref().set_int(lhs / rhs);
            return m_instances.create2_reference(instance);
        }

        inst::instance *
        builtin_function_caller2::int_operator_less(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            const auto lhs = instance.get_value_cref()
                                     .get_int();
            const auto& [rhs] = get_params<alternative_t::int_>(params);
            return m_instances.create2(lhs < rhs);
        }

        inst::instance *
        builtin_function_caller2::int_operator_less_equal(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            const auto lhs = instance.get_value_cref()
                                     .get_int();
            const auto& [rhs] = get_params<alternative_t::int_>(params);
            return m_instances.create2(lhs <= rhs);
        }

        inst::instance *
        builtin_function_caller2::int_operator_greater(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            const auto lhs = instance.get_value_cref()
                                     .get_int();
            const auto& [rhs] = get_params<alternative_t::int_>(params);
            return m_instances.create2(lhs > rhs);
        }

        inst::instance *
        builtin_function_caller2::int_operator_greater_equal(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            const auto lhs = instance.get_value_cref()
                                     .get_int();
            const auto& [rhs] = get_params<alternative_t::int_>(params);
            return m_instances.create2(lhs >= rhs);
        }

        inst::instance *
        builtin_function_caller2::int_operator_equal_equal(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            const auto lhs = instance.get_value_cref()
                                     .get_int();
            const auto& [rhs] = get_params<alternative_t::int_>(params);
            return m_instances.create2(lhs == rhs);
        }

        inst::instance *
        builtin_function_caller2::int_to_string(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            const auto lhs = instance.get_value_cref()
                                     .get_int();
            return m_instances.create2(std::to_string(lhs));
        }

        inst::instance *
        builtin_function_caller2::bool_operator_equal_equal(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            const auto lhs = instance.get_value_cref()
                                     .get_bool();
            const auto& [rhs] = get_params<alternative_t::bool_>(params);
            return m_instances.create2(lhs == rhs);
        }

        inst::instance *
        builtin_function_caller2::bool_operator_pipe_pipe(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            const auto lhs = instance.get_value_cref()
                                     .get_bool();
            const auto& [rhs] = get_params<alternative_t::bool_>(params);
            return m_instances.create2(lhs || rhs);
        }

        inst::instance *
        builtin_function_caller2::bool_operator_amp_amp(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            const auto lhs = instance.get_value_cref()
                                     .get_bool();
            const auto& [rhs] = get_params<alternative_t::bool_>(params);
            return m_instances.create2(lhs && rhs);
        }

        inst::instance *
        builtin_function_caller2::bool_to_string(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            const auto lhs = instance.get_value_cref()
                                     .get_int();
            return m_instances.create2(lhs ? std::string{ "true" } : std::string{ "false" });
        }

        inst::instance *
        builtin_function_caller2::double_ctor(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            instance.get_value_ref().set_double(0.0);
            return &instance;
        }

        inst::instance *
        builtin_function_caller2::int_ctor_double(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            const auto& [param] = get_params<alternative_t::double_>(params);
            instance.get_value_ref() = static_cast<inst::int_t>(param);
            return &instance;
        }

        inst::instance *
        builtin_function_caller2::double_ctor_double(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            const auto& [param] = get_params<alternative_t::double_>(params);
            instance.get_value_ref() = param;
            return &instance;
        }

        inst::instance *
        builtin_function_caller2::double_ctor_int(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            const auto& [param] = get_params<alternative_t::int_>(params);
            instance.get_value_ref() = static_cast<double>(param);
            return &instance;
        }

        inst::instance *
        builtin_function_caller2::double_operator_plus(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            const auto lhs = instance.get_value_cref()
                                     .get_double();
            const auto& [rhs] = get_params<alternative_t::double_>(params);
            return m_instances.create2(lhs + rhs);
        }

        inst::instance *
        builtin_function_caller2::double_operator_minus(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            const auto lhs = instance.get_value_cref()
                                     .get_double();
            const auto& [rhs] = get_params<alternative_t::double_>(params);
            return m_instances.create2(lhs - rhs);
        }

        inst::instance *
        builtin_function_caller2::double_operator_star(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            const auto lhs = instance.get_value_cref()
                                     .get_double();
            const auto& [rhs] = get_params<alternative_t::double_>(params);
            return m_instances.create2(lhs * rhs);
        }

        inst::instance *
        builtin_function_caller2::double_operator_slash(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            const auto lhs = instance.get_value_cref()
                                     .get_double();
            const auto& [rhs] = get_params<alternative_t::double_>(params);
            return m_instances.create2(lhs / rhs);
        }

        inst::instance *
        builtin_function_caller2::double_operator_equal(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            const auto& [rhs] = get_params<alternative_t::double_>(params);
            instance.get_value_ref() = rhs;
            return m_instances.create2_reference(instance);
        }

        inst::instance *
        builtin_function_caller2::double_operator_plus_equal(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            const auto lhs = instance.get_value_cref()
                                     .get_double();
            const auto& [rhs] = get_params<alternative_t::double_>(params);

            instance.get_value_ref().set_double(lhs + rhs);
            return m_instances.create2_reference(instance);
        }

        inst::instance *
        builtin_function_caller2::double_operator_minus_equal(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            const auto lhs = instance.get_value_cref()
                                     .get_double();
            const auto& [rhs] = get_params<alternative_t::double_>(params);

            instance.get_value_ref().set_double(lhs - rhs);
            return m_instances.create2_reference(instance);
        }

        inst::instance *
        builtin_function_caller2::double_operator_star_equal(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            const auto lhs = instance.get_value_cref()
                                     .get_double();
            const auto& [rhs] = get_params<alternative_t::double_>(params);

            instance.get_value_ref().set_double(lhs * rhs);
            return m_instances.create2_reference(instance);
        }

        inst::instance *
        builtin_function_caller2::double_operator_slash_equal(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            const auto lhs = instance.get_value_cref()
                                     .get_double();
            const auto& [rhs] = get_params<alternative_t::double_>(params);

            instance.get_value_ref().set_double(lhs / rhs);
            return m_instances.create2_reference(instance);
        }

        inst::instance *
        builtin_function_caller2::double_operator_less(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            const auto lhs = instance.get_value_cref()
                                     .get_double();
            const auto& [rhs] = get_params<alternative_t::double_>(params);
            return m_instances.create2(lhs < rhs);
        }

        inst::instance *
        builtin_function_caller2::double_operator_less_equal(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            const auto lhs = instance.get_value_cref()
                                     .get_double();
            const auto& [rhs] = get_params<alternative_t::double_>(params);
            return m_instances.create2(lhs <= rhs);
        }

        inst::instance *
        builtin_function_caller2::double_operator_greater(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            const auto lhs = instance.get_value_cref()
                                     .get_double();
            const auto& [rhs] = get_params<alternative_t::double_>(params);
            return m_instances.create2(lhs > rhs);
        }

        inst::instance *
        builtin_function_caller2::double_operator_greater_equal(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            const auto lhs = instance.get_value_cref()
                                     .get_double();
            const auto& [rhs] = get_params<alternative_t::double_>(params);
            return m_instances.create2(lhs >= rhs);
        }

        inst::instance *
        builtin_function_caller2::string_empty(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            const auto& str = instance.get_value_cref().get_string_cref();
            return m_instances.create2(str.empty());
        }

        inst::instance *
        builtin_function_caller2::string_size(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            const auto& str = instance.get_value_cref().get_string_cref();
            const auto size = static_cast<inst::int_t>(str.size());
            return m_instances.create2(size);
        }

        inst::instance *
        builtin_function_caller2::string_ctor(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            instance.get_value_ref().set_string(std::string{});
            return &instance;
        }

        inst::instance *
        builtin_function_caller2::string_ctor_string(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            const auto& [param] = get_params<alternative_t::string>(params);
            instance.get_value_ref().set_string(std::move(param));
            return &instance;
        }

        inst::instance *
        builtin_function_caller2::string_operator_equal_equal(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            const auto& lhs = instance.get_value_cref()
                                     .get_string_cref();
            const auto& [rhs] = get_params<alternative_t::string>(params);
            return m_instances.create2(lhs == rhs);
        }

        inst::instance *
        builtin_function_caller2::string_operator_not_equal(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            const auto& lhs = instance.get_value_cref()
                                      .get_string_cref();
            const auto& [rhs] = get_params<alternative_t::string>(params);
            return m_instances.create2(lhs != rhs);
        }

        inst::instance *
        builtin_function_caller2::string_operator_less(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            const auto& lhs = instance.get_value_cref()
                                      .get_string_cref();
            const auto& [rhs] = get_params<alternative_t::string>(params);
            return m_instances.create2(lhs < rhs);
        }

        inst::instance *
        builtin_function_caller2::string_operator_less_equal(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            const auto& lhs = instance.get_value_cref()
                                      .get_string_cref();
            const auto& [rhs] = get_params<alternative_t::string>(params);
            return m_instances.create2(lhs <= rhs);
        }

        inst::instance *
        builtin_function_caller2::string_operator_greater(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            const auto& lhs = instance.get_value_cref()
                                      .get_string_cref();
            const auto& [rhs] = get_params<alternative_t::string>(params);
            return m_instances.create2(lhs > rhs);
        }

        inst::instance *
        builtin_function_caller2::string_operator_greater_equal(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            const auto& lhs = instance.get_value_cref()
                                      .get_string_cref();
            const auto& [rhs] = get_params<alternative_t::string>(params);
            return m_instances.create2(lhs >= rhs);
        }

        inst::instance *
        builtin_function_caller2::string_operator_plus(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            const auto& lhs = instance.get_value_cref()
                                      .get_string_cref();
            const auto& [rhs] = get_params<alternative_t::string>(params);
            return m_instances.create2(lhs + rhs);
        }

        inst::instance *
        builtin_function_caller2::string_operator_plus_equal(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            auto& lhs = instance.get_value_ref()
                                .get_string_ref();
            const auto& [rhs] = get_params<alternative_t::string>(params);

            lhs += rhs;
            return m_instances.create2_reference(instance);
        }

        inst::instance *
        builtin_function_caller2::string_ctor_string_count(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            auto& str = instance.get_value_ref().get_string_ref();
            const auto& [value, count] = get_params<alternative_t::string, alternative_t::int_>(params);

            str.clear();
            str.reserve(value.size() * count);

            for(auto i = 0u; i < count; ++i)
            {
                str += value;
            }

            return &instance;
        }

        inst::instance *
        builtin_function_caller2::string_clear(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            auto& str = instance.get_value_ref()
                                      .get_string_ref();
            str.clear();
            return m_instances.create2_reference(instance);
        }

        inst::instance *
        builtin_function_caller2::string_insert_pos_str(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            auto& str = instance.get_value_ref().get_string_ref();
            const auto& [position, value] = get_params<alternative_t::int_, alternative_t::string>(params);

            str.insert(position, value);

            return m_instances.create2_reference(instance);
        }

        inst::instance *
        builtin_function_caller2::string_erase_pos(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            auto& str = instance.get_value_ref().get_string_ref();
            const auto& [position] = get_params<alternative_t::int_>(params);
            str.erase(position);
            return m_instances.create2_reference(instance);
        }

        inst::instance *
        builtin_function_caller2::string_erase_pos_count(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            auto& str = instance.get_value_ref().get_string_ref();
            const auto& [position, count] = get_params<alternative_t::int_, alternative_t::int_>(params);
            str.erase(position, count);
            return m_instances.create2_reference(instance);
        }

        inst::instance *
        builtin_function_caller2::string_starts_with(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            const auto& str = instance.get_value_cref().get_string_cref();
            const auto& [value] = get_params<alternative_t::string>(params);

            // Todo: extract to common function with ends_with
            const auto starts_with = [&str, &value = value] // Workaround for capturing variables introduced by structured bindings.
            {
                if (&value == &str)
                {
                    return true; // Very same string.
                }
                if (value.length() > str.length())
                {
                    return false;
                }

                return std::equal(std::cbegin(value), std::cend(value),
                                  std::cbegin(str), std::next(std::cbegin(str), value.size()));
            }();

            return m_instances.create2(starts_with);
        }

        inst::instance *
        builtin_function_caller2::string_ends_with(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            const auto& str = instance.get_value_cref().get_string_cref();
            const auto& [value] = get_params<alternative_t::string>(params);

            const auto ends_with = [&str, &value = value] // Workaround for capturing variables introduced by structured bindings.
            {
                if (&value == &str)
                {
                    return true; // Very same string.
                }
                if (value.length() > str.length())
                {
                    return false;
                }

                return std::equal(std::crbegin(value), std::crend(value),
                                  std::crbegin(str), std::next(std::crbegin(str), value.size()));
            }();

            return m_instances.create2(ends_with);
        }

        inst::instance *
        builtin_function_caller2::string_replace_pos_count_str(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            auto& str = instance.get_value_ref().get_string_ref();
            const auto& [position, count, value] = get_params<alternative_t::int_,
                                                              alternative_t::int_,
                                                              alternative_t::string>(params);
            str.replace(position, count, value);
            return m_instances.create2_reference(instance);
        }

        inst::instance *
        builtin_function_caller2::string_substr_pos(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            const auto& str = instance.get_value_cref().get_string_cref();
            const auto& [position] = get_params<alternative_t::int_>(params);
            return m_instances.create2(str.substr(position));
        }

        inst::instance *
        builtin_function_caller2::string_substr_pos_count(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            const auto& str = instance.get_value_cref().get_string_cref();
            const auto& [position, count] = get_params<alternative_t::int_,
                                                       alternative_t::int_>(params);
            return m_instances.create2(str.substr(position, count));
        }

        inst::instance *
        builtin_function_caller2::string_resize_newsize(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            auto& str = instance.get_value_ref().get_string_ref();
            const auto& [new_size] = get_params<alternative_t::int_>(params);
            str.resize(new_size);
            return m_instances.create2_reference(instance);
        }

        inst::instance *
        builtin_function_caller2::string_resize_newsize_fill(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            auto& str = instance.get_value_ref().get_string_ref();
            const auto& [new_size, fill] = get_params<alternative_t::int_,
                                                      alternative_t::string>(params);
            const auto previous_size = str.length();
            str.resize(new_size);

            if(new_size > previous_size)
            {
                const auto difference = new_size - previous_size;
                const auto fill_length = fill.length();
                for(auto i = 0u; i < difference; ++i)
                {
                    str[previous_size + i] = fill[i % fill_length];
                }
            }

            return m_instances.create2_reference(instance);
        }

        inst::instance *
        builtin_function_caller2::string_find_str(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            const auto& str = instance.get_value_cref().get_string_cref();
            const auto& [value] = get_params<alternative_t::string>(params);
            const auto found_pos = str.find(value);
            const auto pos = string_pos_to_int(found_pos);
            return m_instances.create2(pos);
        }

        inst::instance *
        builtin_function_caller2::string_find_str_pos(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            const auto& str = instance.get_value_cref().get_string_cref();
            const auto& [value, start_pos] = get_params<alternative_t::string,
                                                      alternative_t::int_>(params);
            const auto found_pos = str.find(value, start_pos);
            const auto pos = string_pos_to_int(found_pos);
            return m_instances.create2(pos);
        }

        inst::instance *
        builtin_function_caller2::string_find_not_of_str(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            const auto& str = instance.get_value_cref().get_string_cref();
            const auto& [value] = get_params<alternative_t::string>(params);
            const auto found_pos = str.find_first_not_of(value);
            const auto pos = string_pos_to_int(found_pos);
            return m_instances.create2(pos);
        }

        inst::instance *
        builtin_function_caller2::string_find_not_of_str_pos(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            const auto& str = instance.get_value_cref().get_string_cref();
            const auto& [value, start_pos] = get_params<alternative_t::string,
                                                        alternative_t::int_>(params);
            const auto found_pos = str.find_first_not_of(value, start_pos);
            const auto pos = string_pos_to_int(found_pos);
            return m_instances.create2(pos);
        }

        inst::instance *
        builtin_function_caller2::string_find_last_str(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            const auto& str = instance.get_value_cref().get_string_cref();
            const auto& [value] = get_params<alternative_t::string>(params);
            const auto found_pos = str.rfind(value);
            const auto pos = string_pos_to_int(found_pos);
            return m_instances.create2(pos);
        }

        inst::int_t builtin_function_caller2::string_pos_to_int(std::string::size_type pos) const
        {
            return pos != std::string::npos
                   ? static_cast<inst::int_t>( pos )
                   : inst::int_t{ -1 };
        }

        inst::instance *
        builtin_function_caller2::string_find_last_not_of_str(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            const auto& str = instance.get_value_cref().get_string_cref();
            const auto& [value] = get_params<alternative_t::string>(params);
            const auto found_pos = str.find_last_not_of(value);
            const auto pos = string_pos_to_int(found_pos);
            return m_instances.create2(pos);
        }

        inst::instance *
        builtin_function_caller2::string_contains(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            const auto& str = instance.get_value_cref().get_string_cref();
            const auto& [value] = get_params<alternative_t::string>(params);
            const auto found_pos = str.find(value);
            const auto contains = found_pos != std::string::npos;
            return m_instances.create2(contains);
        }

        inst::instance *
        builtin_function_caller2::string_lower(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            auto& str = instance.get_value_ref().get_string_ref();

            for(auto& c : str)
            {
                c = std::tolower(c);
            }

            return m_instances.create2_reference(instance);
        }

        inst::instance *
        builtin_function_caller2::string_make_lower(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            const auto& str = instance.get_value_cref().get_string_cref();
            std::string result;
            result.reserve(str.length());

            std::transform(std::cbegin(str), std::cend(str),
                           std::back_inserter(result),
                           [](const auto c) { return std::tolower(c, m_utf8_locale); });

            return m_instances.create2(std::move(result));
        }

        inst::instance *
        builtin_function_caller2::string_upper(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            auto& str = instance.get_value_ref().get_string_ref();

            for(auto& c : str)
            {
                c = std::toupper(c);
            }

            return m_instances.create2_reference(instance);
        }

        inst::instance *
        builtin_function_caller2::string_make_upper(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            const auto& str = instance.get_value_cref().get_string_cref();
            std::string result;
            result.reserve(str.length());

            std::transform(std::cbegin(str), std::cend(str),
                           std::back_inserter(result),
                           [](const auto c) { return std::toupper(c, m_utf8_locale); });

            return m_instances.create2(std::move(result));
        }

        inst::instance *
        builtin_function_caller2::version_ctor_major(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            const auto& [major] = get_params<alternative_t::int_>(params);
            instance.get_value_ref().set_version(inst::version_value{ major });
            return &instance;
        }

        inst::instance *
        builtin_function_caller2::version_ctor_major_minor(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            const auto& [major, minor] = get_params<alternative_t::int_,
                                                    alternative_t::int_>(params);
            instance.get_value_ref().set_version(inst::version_value{ major, minor });
            return &instance;
        }

        inst::instance *
        builtin_function_caller2::version_ctor_major_minor_patch(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            const auto& [major, minor, patch] = get_params<alternative_t::int_,
                                                           alternative_t::int_,
                                                           alternative_t::int_>(params);
            instance.get_value_ref().set_version(inst::version_value{ major, minor, patch });
            return &instance;
        }

        inst::instance *
        builtin_function_caller2::version_ctor_major_minor_patch_tweak(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            const auto& [major, minor, patch, tweak] = get_params<alternative_t::int_,
                                                                  alternative_t::int_,
                                                                  alternative_t::int_,
                                                                  alternative_t::int_>(params);
            instance.get_value_ref().set_version(inst::version_value{ major, minor, patch, tweak });
            return &instance;
        }

        inst::instance *
        builtin_function_caller2::version_operator_equal_equal(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            const auto& ver = instance.get_value_cref().get_version_cref();
            const auto& [ver2] = get_params<alternative_t::version>(params);
            return m_instances.create2(ver == ver2);
        }

        inst::instance *
        builtin_function_caller2::version_operator_not_equal(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            const auto& ver = instance.get_value_cref().get_version_cref();
            const auto& [ver2] = get_params<alternative_t::version>(params);
            return m_instances.create2(ver != ver2);
        }

        inst::instance *
        builtin_function_caller2::version_operator_less(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            const auto& ver = instance.get_value_cref().get_version_cref();
            const auto& [ver2] = get_params<alternative_t::version>(params);
            return m_instances.create2(ver < ver2);
        }

        inst::instance *
        builtin_function_caller2::version_operator_less_equal(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            const auto& ver = instance.get_value_cref().get_version_cref();
            const auto& [ver2] = get_params<alternative_t::version>(params);
            return m_instances.create2(ver <= ver2);
        }

        inst::instance *
        builtin_function_caller2::version_operator_greater(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            const auto& ver = instance.get_value_cref().get_version_cref();
            const auto& [ver2] = get_params<alternative_t::version>(params);
            return m_instances.create2(ver > ver2);
        }

        inst::instance *
        builtin_function_caller2::version_operator_greater_equal(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            const auto& ver = instance.get_value_cref().get_version_cref();
            const auto& [ver2] = get_params<alternative_t::version>(params);
            return m_instances.create2(ver >= ver2);
        }

        inst::instance *
        builtin_function_caller2::version_major(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            const auto& ver = instance.get_value_cref().get_version_cref();
            return m_instances.create2(ver.major_());
        }

        inst::instance *
        builtin_function_caller2::version_minor(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            const auto& ver = instance.get_value_cref().get_version_cref();
            return m_instances.create2(ver.minor_());
        }

        inst::instance *
        builtin_function_caller2::version_patch(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            const auto& ver = instance.get_value_cref().get_version_cref();
            return m_instances.create2(ver.patch());
        }

        inst::instance *
        builtin_function_caller2::version_tweak(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            const auto& ver = instance.get_value_cref().get_version_cref();
            return m_instances.create2(ver.tweak());
        }

        inst::instance *
        builtin_function_caller2::version_to_string(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            const auto& ver = instance.get_value_cref().get_version_cref();
            return m_instances.create2(ver.to_string());
        }

        inst::instance *
        builtin_function_caller2::list_ctor(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            auto& list = instance.get_value_ref().get_list_ref();
            list = inst::list_value{};
            return m_instances.create2_reference(instance);
        }

        inst::instance *
        builtin_function_caller2::list_size(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            const auto& list = instance.get_value_cref().get_list_cref();
            return m_instances.create2(static_cast<inst::int_t>(list.size()));
        }

        inst::instance *
        builtin_function_caller2::list_ctor_list(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            auto& list = instance.get_value_ref().get_list_ref();
            const auto& [value] = get_params<alternative_t::list>(params);
            list = inst::list_value{ value };
            return m_instances.create2_reference(instance);
        }

        inst::instance *
        builtin_function_caller2::list_ctor_value(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            auto& list = instance.get_value_ref().get_list_ref();
            list.push_back(params[0]->copy());
            return m_instances.create2_reference(instance);
        }

        inst::instance *
        builtin_function_caller2::list_ctor_value_count(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            auto& list = instance.get_value_ref().get_list_ref();
            const auto count = params[1]->get_value_cref().get_int();

            for(auto i = 0u; i < count; ++i)
            {
                list.push_back(params[0]->copy());
            }

            return m_instances.create2_reference(instance);
        }

        inst::instance *
        builtin_function_caller2::list_push_back_value(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            auto& list = instance.get_value_ref().get_list_ref();
            list.push_back(params[0]->copy());
            return m_instances.create2_void();
        }

        inst::instance *
        builtin_function_caller2::list_push_back_list(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            auto& list = instance.get_value_ref().get_list_ref();
            const auto& [list2] = get_params<alternative_t::list>(params);
            list.push_back(list2);
            return m_instances.create2_void();
        }

        inst::instance *
        builtin_function_caller2::list_push_front_value(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            auto& list = instance.get_value_ref().get_list_ref();
            list.push_front(params[0]->copy());
            return m_instances.create2_void();
        }

        inst::instance *
        builtin_function_caller2::list_push_front_list(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            auto& list = instance.get_value_ref().get_list_ref();
            const auto& [list2] = get_params<alternative_t::list>(params);
            list.push_front(list2);
            return m_instances.create2_void();
        }

        inst::instance *
        builtin_function_caller2::list_pop_back(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            auto& list = instance.get_value_ref().get_list_ref();
            list.pop_back();
            return m_instances.create2_void();
        }

        inst::instance *
        builtin_function_caller2::list_pop_front(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            auto& list = instance.get_value_ref().get_list_ref();
            list.pop_front();
            return m_instances.create2_void();
        }

        inst::instance *
        builtin_function_caller2::list_at(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            auto& list = instance.get_value_ref().get_list_ref();
            const auto& [position] = get_params<alternative_t::int_>(params);
            auto& instance_at = list.at(position);
            return m_instances.create2_reference(instance_at);
        }

        inst::instance *
        builtin_function_caller2::list_front(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            auto& list = instance.get_value_ref().get_list_ref();
            auto& instance_at_front = list.front();
            return m_instances.create2_reference(instance_at_front);
        }

        inst::instance *
        builtin_function_caller2::list_back(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            auto& list = instance.get_value_ref().get_list_ref();
            auto& instance_at_back = list.back();
            return m_instances.create2_reference(instance_at_back);
        }

        inst::instance *
        builtin_function_caller2::list_insert_pos_value(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            auto& list = instance.get_value_ref().get_list_ref();
            const auto& [position] = get_params<alternative_t::int_>(params);
            list.insert(position, params[1]->copy());
            return m_instances.create2_void();
        }

        inst::instance *
        builtin_function_caller2::list_insert_pos_list(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            auto& list = instance.get_value_ref().get_list_ref();
            const auto& [position, list2] = get_params<alternative_t::int_, alternative_t::list>(params);
            list.insert(position, list2);
            return m_instances.create2_void();
        }

        inst::instance *
        builtin_function_caller2::list_erase_pos(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            auto& list = instance.get_value_ref().get_list_ref();
            const auto& [position] = get_params<alternative_t::int_>(params);
            list.erase(position);
            return m_instances.create2_void();
        }

        inst::instance *
        builtin_function_caller2::list_erase_pos_count(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            auto& list = instance.get_value_ref().get_list_ref();
            const auto& [position, count] = get_params<alternative_t::int_, alternative_t::int_>(params);
            list.erase(position, count);
            return m_instances.create2_void();
        }

        inst::instance *
        builtin_function_caller2::list_remove_value(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            auto& list = instance.get_value_ref().get_list_ref();
            const auto& value = *params[0];
            const auto removed_count = list.remove(value);
            return m_instances.create2(removed_count);
        }

        inst::instance *
        builtin_function_caller2::list_remove_value_count(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            auto& list = instance.get_value_ref().get_list_ref();
            const auto& value = *params[0];
            const auto& count = params[1]->get_value_cref().get_int();
            const auto removed_count = list.remove(value, count);
            return m_instances.create2(removed_count);
        }

        inst::instance *
        builtin_function_caller2::list_remove_last_value_count(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            auto& list = instance.get_value_ref().get_list_ref();
            const auto& value = *params[0];
            const auto& count = params[1]->get_value_cref().get_int();
            const auto removed_count = list.remove_last(value, count);
            return m_instances.create2(removed_count);
        }

        inst::instance *
        builtin_function_caller2::list_clear(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            auto& list = instance.get_value_ref().get_list_ref();
            list.clear();
            return m_instances.create2_void();
        }

        inst::instance *
        builtin_function_caller2::list_resize(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            const auto& list_type = *dynamic_cast<const sema::homogeneous_generic_type*>(&instance.get_sema_type());
            const auto& value_type = list_type.value_type();

            auto& list = instance.get_value_ref().get_list_ref();
            const auto& [new_size] = get_params<alternative_t::int_>(params);

            const auto fill_value = new_size > list.size() ? m_instances.create2(value_type) : nullptr;
            list.resize(new_size, fill_value);
            return m_instances.create2_void();
        }

        inst::instance *
        builtin_function_caller2::list_sort(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            auto& list = instance.get_value_ref().get_list_ref();
            list.sort();
            return m_instances.create2_void();
        }

        inst::instance *
        builtin_function_caller2::list_reverse(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            auto& list = instance.get_value_ref().get_list_ref();
            list.reverse();
            return m_instances.create2_void();
        }

        inst::instance *
        builtin_function_caller2::list_min(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            const auto& list = instance.get_value_cref().get_list_cref();
            const auto min_element_index = list.min();
            return m_instances.create2(min_element_index);
        }

        inst::instance *
        builtin_function_caller2::list_max(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            const auto& list = instance.get_value_cref().get_list_cref();
            const auto max_element_index = list.max();
            return m_instances.create2(max_element_index);
        }

        inst::instance *
        builtin_function_caller2::list_sublist_pos(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            const auto& list = instance.get_value_cref().get_list_cref();
            const auto& [position] = get_params<alternative_t::int_>(params);
            auto sublist = list.sublist(position);
            return m_instances.create2(instance.get_sema_type(), std::move(sublist));
        }

        inst::instance *
        builtin_function_caller2::list_sublist_pos_count(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            const auto& list = instance.get_value_cref().get_list_cref();
            const auto& [position, count] = get_params<alternative_t::int_, alternative_t::int_>(params);
            auto sublist = list.sublist(position, count);
            return m_instances.create2(std::move(sublist));
        }

        inst::instance *
        builtin_function_caller2::list_empty(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            const auto& list = instance.get_value_cref().get_list_cref();
            const auto is_empty = list.empty();
            return m_instances.create2(is_empty);
        }

        inst::instance *
        builtin_function_caller2::list_find_value(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            const auto& list = instance.get_value_cref().get_list_cref();
            const auto& value = *params[0];
            const auto found_index = list.find(value);
            return m_instances.create2(found_index);
        }

        inst::instance *
        builtin_function_caller2::list_find_value_pos(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            const auto& list = instance.get_value_cref().get_list_cref();
            const auto& value = *params[0];
            const auto position = params[1]->get_value_cref().get_int();
            const auto found_index = list.find(value, position);
            return m_instances.create2(found_index);
        }

        inst::instance *
        builtin_function_caller2::list_operator_plus_value(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            auto list_copy = instance.get_value_cref().get_list_cref();
            auto value = params[0]->copy();
            list_copy.push_back(std::move(value));
            return m_instances.create2(std::move(list_copy));
        }

        inst::instance *
        builtin_function_caller2::list_operator_plus_list(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            auto list_copy = instance.get_value_cref().get_list_cref();
            auto [list_to_append] = get_params<alternative_t::list>(params);
            list_copy.push_back(std::move(list_to_append));
            return m_instances.create2(std::move(list_copy));
        }

        inst::instance *
        builtin_function_caller2::list_operator_plus_equal_value(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            auto& list = instance.get_value_ref().get_list_ref();
            auto value = params[0]->copy();
            list.push_back(std::move(value));
            return m_instances.create2_reference(instance);
        }

        inst::instance *
        builtin_function_caller2::list_operator_plus_equal_list(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            auto& list = instance.get_value_ref().get_list_ref();
            auto [list_to_append] = get_params<alternative_t::list>(params);
            list.push_back(std::move(list_to_append));
            return m_instances.create2_reference(instance);
        }
    }
}
