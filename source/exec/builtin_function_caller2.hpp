#pragma once

#include <memory>
#include <vector>
#include <locale>
#include "exec/instance/int_alias.hpp"

namespace cmsl
{
    namespace sema
    {
        enum class builtin_function_kind;
    }

    namespace exec
    {
        namespace inst
        {
            class instance;
            class instances_holder_interface;
        }

        class builtin_function_caller2
        {
        public:
            using params_t = std::vector<inst::instance*>;

            explicit builtin_function_caller2(inst::instances_holder_interface& instances);

            std::unique_ptr<inst::instance> call_member(inst::instance& instance, sema::builtin_function_kind function_kind, const params_t& params);

        private:
            inst::instance* bool_ctor(inst::instance& instance, const params_t& params);
            inst::instance* bool_ctor_bool(inst::instance& instance, const params_t& params);
            inst::instance* bool_ctor_int(inst::instance& instance, const params_t& params);
            inst::instance* bool_operator_equal(inst::instance& instance, const params_t& params);
            inst::instance* bool_operator_equal_equal(inst::instance& instance, const params_t& params);
            inst::instance* bool_operator_pipe_pipe(inst::instance& instance, const params_t& params);
            inst::instance* bool_operator_amp_amp(inst::instance& instance, const params_t& params);
            inst::instance* bool_to_string(inst::instance& instance, const params_t& params);

            inst::instance* int_ctor(inst::instance& instance, const params_t& params);
            inst::instance* int_ctor_bool(inst::instance& instance, const params_t& params);
            inst::instance* int_ctor_int(inst::instance& instance, const params_t& params);
            inst::instance* int_ctor_double(inst::instance& instance, const params_t& params);
            inst::instance* int_operator_plus(inst::instance& instance, const params_t& params);
            inst::instance* int_operator_minus(inst::instance& instance, const params_t& params);
            inst::instance* int_operator_star(inst::instance& instance, const params_t& params);
            inst::instance* int_operator_slash(inst::instance& instance, const params_t& params);
            inst::instance* int_operator_equal(inst::instance& instance, const params_t& params);
            inst::instance* int_operator_plus_equal(inst::instance& instance, const params_t& params);
            inst::instance* int_operator_minus_equal(inst::instance& instance, const params_t& params);
            inst::instance* int_operator_star_equal(inst::instance& instance, const params_t& params);
            inst::instance* int_operator_slash_equal(inst::instance& instance, const params_t& params);
            inst::instance* int_operator_less(inst::instance& instance, const params_t& params);
            inst::instance* int_operator_less_equal(inst::instance& instance, const params_t& params);
            inst::instance* int_operator_greater(inst::instance& instance, const params_t& params);
            inst::instance* int_operator_greater_equal(inst::instance& instance, const params_t& params);
            inst::instance* int_operator_equal_equal(inst::instance& instance, const params_t& params);
            inst::instance* int_to_string(inst::instance& instance, const params_t& params);

            inst::instance* double_ctor(inst::instance& instance, const params_t& params);
            inst::instance* double_ctor_double(inst::instance& instance, const params_t& params);
            inst::instance* double_ctor_int(inst::instance& instance, const params_t& params);
            inst::instance* double_operator_plus(inst::instance& instance, const params_t& params);
            inst::instance* double_operator_minus(inst::instance& instance, const params_t& params);
            inst::instance* double_operator_star(inst::instance& instance, const params_t& params);
            inst::instance* double_operator_slash(inst::instance& instance, const params_t& params);
            inst::instance* double_operator_equal(inst::instance& instance, const params_t& params);
            inst::instance* double_operator_plus_equal(inst::instance& instance, const params_t& params);
            inst::instance* double_operator_minus_equal(inst::instance& instance, const params_t& params);
            inst::instance* double_operator_star_equal(inst::instance& instance, const params_t& params);
            inst::instance* double_operator_slash_equal(inst::instance& instance, const params_t& params);
            inst::instance* double_operator_less(inst::instance& instance, const params_t& params);
            inst::instance* double_operator_less_equal(inst::instance& instance, const params_t& params);
            inst::instance* double_operator_greater(inst::instance& instance, const params_t& params);
            inst::instance* double_operator_greater_equal(inst::instance& instance, const params_t& params);

            inst::instance* string_ctor(inst::instance& instance, const params_t& params);
            inst::instance* string_ctor_string(inst::instance& instance, const params_t& params);
            inst::instance* string_empty(inst::instance& instance, const params_t& params);
            inst::instance* string_size(inst::instance& instance, const params_t& params);
            inst::instance* string_operator_equal_equal(inst::instance& instance, const params_t& params);
            inst::instance* string_operator_not_equal(inst::instance& instance, const params_t& params);
            inst::instance* string_operator_less(inst::instance& instance, const params_t& params);
            inst::instance* string_operator_less_equal(inst::instance& instance, const params_t& params);
            inst::instance* string_operator_greater(inst::instance& instance, const params_t& params);
            inst::instance* string_operator_greater_equal(inst::instance& instance, const params_t& params);
            inst::instance* string_operator_plus(inst::instance& instance, const params_t& params);
            inst::instance* string_operator_plus_equal(inst::instance& instance, const params_t& params);
            inst::instance* string_ctor_string_count(inst::instance& instance, const params_t& params);
            inst::instance* string_clear(inst::instance& instance, const params_t& params);
            inst::instance* string_insert_pos_str(inst::instance& instance, const params_t& params);
            inst::instance* string_erase_pos(inst::instance& instance, const params_t& params);
            inst::instance* string_erase_pos_count(inst::instance& instance, const params_t& params);
            inst::instance* string_starts_with(inst::instance& instance, const params_t& params);
            inst::instance* string_ends_with(inst::instance& instance, const params_t& params);
            inst::instance* string_replace_pos_count_str(inst::instance& instance, const params_t& params);
            inst::instance* string_substr_pos(inst::instance& instance, const params_t& params);
            inst::instance* string_substr_pos_count(inst::instance& instance, const params_t& params);
            inst::instance* string_resize_newsize(inst::instance& instance, const params_t& params);
            inst::instance* string_resize_newsize_fill(inst::instance& instance, const params_t& params);
            inst::instance* string_find_str(inst::instance& instance, const params_t& params);
            inst::instance* string_find_str_pos(inst::instance& instance, const params_t& params);
            inst::instance* string_find_not_of_str(inst::instance& instance, const params_t& params);
            inst::instance* string_find_not_of_str_pos(inst::instance& instance, const params_t& params);
            inst::instance* string_find_last_str(inst::instance& instance, const params_t& params);
            inst::instance* string_find_last_not_of_str(inst::instance& instance, const params_t& params);
            inst::instance* string_contains(inst::instance& instance, const params_t& params);
            inst::instance* string_lower(inst::instance& instance, const params_t& params);
            inst::instance* string_make_lower(inst::instance& instance, const params_t& params);
            inst::instance* string_upper(inst::instance& instance, const params_t& params);
            inst::instance* string_make_upper(inst::instance& instance, const params_t& params);

            inst::instance* version_ctor_major(inst::instance& instance, const params_t& params);
            inst::instance* version_ctor_major_minor(inst::instance& instance, const params_t& params);
            inst::instance* version_ctor_major_minor_patch(inst::instance& instance, const params_t& params);
            inst::instance* version_ctor_major_minor_patch_tweak(inst::instance& instance, const params_t& params);
            inst::instance* version_operator_equal_equal(inst::instance& instance, const params_t& params);
            inst::instance* version_operator_not_equal(inst::instance& instance, const params_t& params);
            inst::instance* version_operator_less(inst::instance& instance, const params_t& params);
            inst::instance* version_operator_less_equal(inst::instance& instance, const params_t& params);
            inst::instance* version_operator_greater(inst::instance& instance, const params_t& params);
            inst::instance* version_operator_greater_equal(inst::instance& instance, const params_t& params);
            inst::instance* version_major(inst::instance& instance, const params_t& params);
            inst::instance* version_minor(inst::instance& instance, const params_t& params);
            inst::instance* version_patch(inst::instance& instance, const params_t& params);
            inst::instance* version_tweak(inst::instance& instance, const params_t& params);
            inst::instance* version_to_string(inst::instance& instance, const params_t& params);

            inst::instance* list_size(inst::instance& instance, const params_t& params);

        private:
            inst::int_t string_pos_to_int(std::string::size_type pos) const;

        private:
            static const std::locale m_utf8_locale;

            inst::instances_holder_interface& m_instances;
        };
    }
}



