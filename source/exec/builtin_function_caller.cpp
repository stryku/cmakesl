#include "exec/builtin_function_caller.hpp"
#include "builtin_function_caller.hpp"
#include "common/assert.hpp"
#include "exec/extern_argument_parser.hpp"
#include "exec/instance/instance.hpp"
#include "exec/instance/instances_holder.hpp"
#include "exec/instance/list_value_utils.hpp"
#include "exec/parameter_alternatives_getter.hpp"
#include "sema/builtin_function_kind.hpp"
#include "sema/builtin_types_accessor.hpp"
#include "sema/cmake_namespace_types_accessor.hpp"
#include "sema/enum_type.hpp"

#include "cmake_facade.hpp"
#include <algorithm>
#include <sema/homogeneous_generic_type.hpp>

#define CASE_BUILTIN_MEMBER_FUNCTION_CALL(function)                           \
  case sema::builtin_function_kind::function: {                               \
    result = function(instance, params);                                      \
  } break

#define CASE_BUILTIN_FUNCTION_CALL(function)                                  \
  case sema::builtin_function_kind::function: {                               \
    result = function(params);                                                \
  } break

namespace cmsl::exec {
template <auto... Alternatives, typename Params>
decltype(auto) get_params(Params&& params)
{
  using params_getter_t = parameter_alternatives_getter<Alternatives...>;
  return params_getter_t::get(std::forward<Params>(params));
}

using alternative_t = inst::instance_value_alternative;

const std::locale builtin_function_caller::m_utf8_locale("en_US.utf8");

builtin_function_caller::builtin_function_caller(
  facade::cmake_facade& cmake_facade,
  inst::instances_holder_interface& instances,
  const sema::builtin_types_accessor& builtin_types)
  : m_cmake_facade{ cmake_facade }
  , m_instances{ instances }
  , m_builtin_types{ builtin_types }
{
}

std::unique_ptr<inst::instance> builtin_function_caller::call_member(
  inst::instance& instance, sema::builtin_function_kind function_kind,
  const params_t& params)
{
  inst::instance* result{ nullptr };

  switch (function_kind) {
    // bool
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(bool_ctor);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(bool_ctor_bool);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(bool_ctor_int);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(bool_operator_equal);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(bool_operator_equal_equal);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(bool_operator_pipe_pipe);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(bool_operator_amp_amp);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(bool_operator_unary_exclaim);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(bool_to_string);

    // int
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(int_ctor);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(int_ctor_bool);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(int_ctor_int);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(int_ctor_double);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(int_operator_plus);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(int_operator_unary_plusplus);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(int_operator_minus);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(int_operator_unary_minus);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(int_operator_unary_minusminus);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(int_operator_star);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(int_operator_slash);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(int_operator_equal);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(int_operator_plus_equal);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(int_operator_minus_equal);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(int_operator_star_equal);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(int_operator_slash_equal);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(int_operator_less);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(int_operator_less_equal);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(int_operator_greater);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(int_operator_greater_equal);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(int_operator_equal_equal);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(int_to_string);

    // double
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(double_ctor);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(double_ctor_double);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(double_ctor_int);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(double_operator_plus);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(double_operator_unary_plusplus);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(double_operator_minus);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(double_operator_unary_minus);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(double_operator_unary_minusminus);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(double_operator_star);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(double_operator_slash);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(double_operator_equal);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(double_operator_plus_equal);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(double_operator_minus_equal);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(double_operator_star_equal);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(double_operator_slash_equal);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(double_operator_less);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(double_operator_less_equal);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(double_operator_greater);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(double_operator_greater_equal);

    // string
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(string_ctor);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(string_ctor_string);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(string_ctor_string_count);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(string_empty);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(string_size);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(string_operator_equal_equal);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(string_operator_not_equal);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(string_operator_less);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(string_operator_less_equal);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(string_operator_greater);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(string_operator_greater_equal);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(string_operator_plus);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(string_operator_plus_equal);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(string_clear);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(string_insert_pos_str);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(string_erase_pos);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(string_erase_pos_count);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(string_starts_with);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(string_ends_with);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(string_replace_pos_count_str);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(string_substr_pos);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(string_substr_pos_count);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(string_resize_newsize);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(string_resize_newsize_fill);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(string_find_str);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(string_find_str_pos);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(string_find_not_of_str);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(string_find_not_of_str_pos);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(string_find_last_str);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(string_find_last_not_of_str);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(string_contains);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(string_lower);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(string_make_lower);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(string_upper);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(string_make_upper);

    // version
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(version_ctor_major);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(version_ctor_major_minor);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(version_ctor_major_minor_patch);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(version_ctor_major_minor_patch_tweak);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(version_operator_equal_equal);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(version_operator_not_equal);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(version_operator_less);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(version_operator_less_equal);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(version_operator_greater);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(version_operator_greater_equal);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(version_major);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(version_minor);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(version_patch);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(version_tweak);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(version_to_string);

    // extern
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(extern_constructor_name);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(extern_has_value);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(extern_value);

    // list
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(list_ctor);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(list_ctor_list);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(list_ctor_value);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(list_ctor_value_count);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(list_push_back_value);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(list_push_back_list);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(list_push_front_value);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(list_push_front_list);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(list_pop_back);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(list_pop_front);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(list_at);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(list_front);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(list_back);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(list_insert_pos_value);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(list_insert_pos_list);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(list_erase_pos);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(list_erase_pos_count);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(list_remove_value);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(list_remove_value_count);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(list_remove_last_value_count);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(list_clear);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(list_resize);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(list_sort);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(list_reverse);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(list_min);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(list_max);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(list_sublist_pos);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(list_sublist_pos_count);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(list_size);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(list_empty);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(list_find_value);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(list_find_value_pos);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(list_operator_plus_value);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(list_operator_plus_list);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(list_operator_plus_equal_value);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(list_operator_plus_equal_list);

    CASE_BUILTIN_MEMBER_FUNCTION_CALL(project_ctor_name);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(project_name);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(project_add_executable);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(project_add_library);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(project_find_library);

    CASE_BUILTIN_MEMBER_FUNCTION_CALL(library_name);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(library_link_to);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(library_link_to_visibility);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(library_include_directories);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(library_include_directories_visibility);

    CASE_BUILTIN_MEMBER_FUNCTION_CALL(executable_name);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(executable_link_to);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(executable_link_to_visibility);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(executable_include_directories);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(
      executable_include_directories_visibility);

    CASE_BUILTIN_MEMBER_FUNCTION_CALL(enum_to_string);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(enum_operator_equal);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(enum_operator_equalequal);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(enum_operator_exclaimequal);

    CASE_BUILTIN_MEMBER_FUNCTION_CALL(user_type_operator_equal);

    CASE_BUILTIN_MEMBER_FUNCTION_CALL(option_ctor_description);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(option_ctor_description_value);
    CASE_BUILTIN_MEMBER_FUNCTION_CALL(option_value);

    default:
      CMSL_UNREACHABLE("Calling unimplemented member function");
      return nullptr;
  }

  return m_instances.gather_ownership(result);
}

std::unique_ptr<inst::instance> builtin_function_caller::call(
  sema::builtin_function_kind function_kind,
  const builtin_function_caller::params_t& params)
{
  inst::instance* result{ nullptr };

  switch (function_kind) {
    CASE_BUILTIN_FUNCTION_CALL(cmake_minimum_required);
    CASE_BUILTIN_FUNCTION_CALL(cmake_message);
    CASE_BUILTIN_FUNCTION_CALL(cmake_warning);
    CASE_BUILTIN_FUNCTION_CALL(cmake_error);
    CASE_BUILTIN_FUNCTION_CALL(cmake_fatal_error);
    CASE_BUILTIN_FUNCTION_CALL(cmake_get_cxx_compiler_info);
    CASE_BUILTIN_FUNCTION_CALL(cmake_install_executable);
    CASE_BUILTIN_FUNCTION_CALL(cmake_install_library);
    CASE_BUILTIN_FUNCTION_CALL(cmake_enable_ctest);
    CASE_BUILTIN_FUNCTION_CALL(cmake_add_test);
    CASE_BUILTIN_FUNCTION_CALL(cmake_root_source_dir);
    CASE_BUILTIN_FUNCTION_CALL(cmake_current_binary_dir);
    CASE_BUILTIN_FUNCTION_CALL(cmake_add_custom_command);
    CASE_BUILTIN_FUNCTION_CALL(cmake_make_directory);
    CASE_BUILTIN_FUNCTION_CALL(cmake_set_old_style_variable);

    default:
      CMSL_UNREACHABLE("Calling unimplemented function");
      return nullptr;
  }

  return m_instances.gather_ownership(result);
}

inst::instance* builtin_function_caller::int_operator_minus(
  inst::instance& instance, const builtin_function_caller::params_t& params)
{
  const auto lhs = instance.value_cref().get_int();
  const auto& [rhs] = get_params<alternative_t::int_>(params);
  return m_instances.create(lhs - rhs);
}

inst::instance* builtin_function_caller::int_operator_unary_minus(
  inst::instance& instance, const builtin_function_caller::params_t& params)
{
  const auto value = instance.value_cref().get_int();
  return m_instances.create(-1 * value);
}

inst::instance* builtin_function_caller::int_operator_unary_minusminus(
  inst::instance& instance, const builtin_function_caller::params_t& params)
{
  const auto val = instance.value_cref().get_int();
  instance.value_accessor().access().set_int(val - 1);
  return m_instances.create_reference(instance);
}

inst::instance* builtin_function_caller::int_operator_equal(
  inst::instance& instance, const builtin_function_caller::params_t& params)
{
  const auto& [rhs] = get_params<alternative_t::int_>(params);
  instance.value_accessor().access().set_int(rhs);
  return m_instances.create_reference(instance);
}

inst::instance* builtin_function_caller::int_ctor_bool(
  inst::instance& instance, const builtin_function_caller::params_t& params)
{
  const auto& [param] = get_params<alternative_t::bool_>(params);
  instance.value_accessor().access().set_int(static_cast<int_t>(param));
  return &instance;
}

inst::instance* builtin_function_caller::bool_ctor(
  inst::instance& instance, const builtin_function_caller::params_t&)
{
  instance.value_accessor().access().set_bool(false);
  return &instance;
}

inst::instance* builtin_function_caller::bool_ctor_bool(
  inst::instance& instance, const builtin_function_caller::params_t& params)
{
  const auto& [param] = get_params<alternative_t::bool_>(params);
  instance.value_accessor().access().set_bool(param);
  return &instance;
}

inst::instance* builtin_function_caller::bool_ctor_int(
  inst::instance& instance, const builtin_function_caller::params_t& params)
{
  const auto& [param] = get_params<alternative_t::int_>(params);
  instance.value_accessor().access().set_bool(static_cast<bool>(param));
  return &instance;
}

inst::instance* builtin_function_caller::bool_operator_equal(
  inst::instance& instance, const builtin_function_caller::params_t& params)
{
  const auto& [rhs] = get_params<alternative_t::bool_>(params);
  instance.value_accessor().access().set_bool(rhs);
  return m_instances.create_reference(instance);
}

inst::instance* builtin_function_caller::int_ctor(
  inst::instance& instance, const builtin_function_caller::params_t&)
{
  instance.value_accessor().access().set_int(int_t{ 0 });
  return &instance;
}

inst::instance* builtin_function_caller::int_ctor_int(
  inst::instance& instance, const builtin_function_caller::params_t& params)
{
  const auto& [rhs] = get_params<alternative_t::int_>(params);
  instance.value_accessor().access().set_int(rhs);
  return &instance;
}

inst::instance* builtin_function_caller::int_operator_plus(
  inst::instance& instance, const builtin_function_caller::params_t& params)
{
  const auto lhs = instance.value_cref().get_int();
  const auto& [rhs] = get_params<alternative_t::int_>(params);
  return m_instances.create(lhs + rhs);
}

inst::instance* builtin_function_caller::int_operator_unary_plusplus(
  inst::instance& instance, const builtin_function_caller::params_t& params)
{
  const auto val = instance.value_cref().get_int();
  instance.value_accessor().access().set_int(int_t{ val + 1 });
  return m_instances.create_reference(instance);
}

inst::instance* builtin_function_caller::int_operator_plus_equal(
  inst::instance& instance, const builtin_function_caller::params_t& params)
{
  const auto lhs = instance.value_cref().get_int();
  const auto& [rhs] = get_params<alternative_t::int_>(params);

  instance.value_accessor().access().set_int(lhs + rhs);
  return m_instances.create_reference(instance);
}

inst::instance* builtin_function_caller::int_operator_minus_equal(
  inst::instance& instance, const builtin_function_caller::params_t& params)
{
  const auto lhs = instance.value_cref().get_int();
  const auto& [rhs] = get_params<alternative_t::int_>(params);

  instance.value_accessor().access().set_int(lhs - rhs);
  return m_instances.create_reference(instance);
}

inst::instance* builtin_function_caller::int_operator_star(
  inst::instance& instance, const builtin_function_caller::params_t& params)
{
  const auto lhs = instance.value_cref().get_int();
  const auto& [rhs] = get_params<alternative_t::int_>(params);
  return m_instances.create(lhs * rhs);
}

inst::instance* builtin_function_caller::int_operator_slash(
  inst::instance& instance, const builtin_function_caller::params_t& params)
{
  const auto lhs = instance.value_cref().get_int();
  const auto& [rhs] = get_params<alternative_t::int_>(params);
  return m_instances.create(lhs / rhs);
}

inst::instance* builtin_function_caller::int_operator_star_equal(
  inst::instance& instance, const builtin_function_caller::params_t& params)
{
  const auto lhs = instance.value_cref().get_int();
  const auto& [rhs] = get_params<alternative_t::int_>(params);

  instance.value_accessor().access().set_int(lhs * rhs);
  return m_instances.create_reference(instance);
}

inst::instance* builtin_function_caller::int_operator_slash_equal(
  inst::instance& instance, const builtin_function_caller::params_t& params)
{
  const auto lhs = instance.value_cref().get_int();
  const auto& [rhs] = get_params<alternative_t::int_>(params);

  instance.value_accessor().access().set_int(lhs / rhs);
  return m_instances.create_reference(instance);
}

inst::instance* builtin_function_caller::int_operator_less(
  inst::instance& instance, const builtin_function_caller::params_t& params)
{
  const auto lhs = instance.value_cref().get_int();
  const auto& [rhs] = get_params<alternative_t::int_>(params);
  return m_instances.create(lhs < rhs);
}

inst::instance* builtin_function_caller::int_operator_less_equal(
  inst::instance& instance, const builtin_function_caller::params_t& params)
{
  const auto lhs = instance.value_cref().get_int();
  const auto& [rhs] = get_params<alternative_t::int_>(params);
  return m_instances.create(lhs <= rhs);
}

inst::instance* builtin_function_caller::int_operator_greater(
  inst::instance& instance, const builtin_function_caller::params_t& params)
{
  const auto lhs = instance.value_cref().get_int();
  const auto& [rhs] = get_params<alternative_t::int_>(params);
  return m_instances.create(lhs > rhs);
}

inst::instance* builtin_function_caller::int_operator_greater_equal(
  inst::instance& instance, const builtin_function_caller::params_t& params)
{
  const auto lhs = instance.value_cref().get_int();
  const auto& [rhs] = get_params<alternative_t::int_>(params);
  return m_instances.create(lhs >= rhs);
}

inst::instance* builtin_function_caller::int_operator_equal_equal(
  inst::instance& instance, const builtin_function_caller::params_t& params)
{
  const auto lhs = instance.value_cref().get_int();
  const auto& [rhs] = get_params<alternative_t::int_>(params);
  return m_instances.create(lhs == rhs);
}

inst::instance* builtin_function_caller::int_to_string(
  inst::instance& instance, const builtin_function_caller::params_t&)
{
  const auto lhs = instance.value_cref().get_int();
  return m_instances.create(std::to_string(lhs));
}

inst::instance* builtin_function_caller::bool_operator_equal_equal(
  inst::instance& instance, const builtin_function_caller::params_t& params)
{
  const auto lhs = instance.value_cref().get_bool();
  const auto& [rhs] = get_params<alternative_t::bool_>(params);
  return m_instances.create(lhs == rhs);
}

inst::instance* builtin_function_caller::bool_operator_pipe_pipe(
  inst::instance& instance, const builtin_function_caller::params_t& params)
{
  const auto lhs = instance.value_cref().get_bool();
  const auto& [rhs] = get_params<alternative_t::bool_>(params);
  return m_instances.create(lhs || rhs);
}

inst::instance* builtin_function_caller::bool_operator_amp_amp(
  inst::instance& instance, const builtin_function_caller::params_t& params)
{
  const auto lhs = instance.value_cref().get_bool();
  const auto& [rhs] = get_params<alternative_t::bool_>(params);
  return m_instances.create(lhs && rhs);
}

inst::instance* builtin_function_caller::bool_operator_unary_exclaim(
  inst::instance& instance, const builtin_function_caller::params_t& params)
{
  const auto lhs = instance.value_cref().get_bool();
  return m_instances.create(!lhs);
}

inst::instance* builtin_function_caller::bool_to_string(
  inst::instance& instance, const builtin_function_caller::params_t&)
{
  const auto lhs = instance.value_cref().get_int();
  return m_instances.create(lhs ? std::string{ "true" }
                                : std::string{ "false" });
}

inst::instance* builtin_function_caller::double_ctor(
  inst::instance& instance, const builtin_function_caller::params_t&)
{
  instance.value_accessor().access().set_double(0.0);
  return &instance;
}

inst::instance* builtin_function_caller::int_ctor_double(
  inst::instance& instance, const builtin_function_caller::params_t& params)
{
  const auto& [param] = get_params<alternative_t::double_>(params);
  instance.value_accessor().access().set_int(static_cast<int_t>(param));
  return &instance;
}

inst::instance* builtin_function_caller::double_ctor_double(
  inst::instance& instance, const builtin_function_caller::params_t& params)
{
  const auto& [param] = get_params<alternative_t::double_>(params);
  instance.value_accessor().access().set_double(param);
  return &instance;
}

inst::instance* builtin_function_caller::double_ctor_int(
  inst::instance& instance, const builtin_function_caller::params_t& params)
{
  const auto& [param] = get_params<alternative_t::int_>(params);
  instance.value_accessor().access().set_double(static_cast<double>(param));
  return &instance;
}

inst::instance* builtin_function_caller::double_operator_plus(
  inst::instance& instance, const builtin_function_caller::params_t& params)
{
  const auto lhs = instance.value_cref().get_double();
  const auto& [rhs] = get_params<alternative_t::double_>(params);
  return m_instances.create(lhs + rhs);
}

inst::instance* builtin_function_caller::double_operator_unary_plusplus(
  inst::instance& instance, const builtin_function_caller::params_t& params)
{
  const auto val = instance.value_cref().get_double();
  instance.value_accessor().access().set_double(val + 1.0);
  return m_instances.create_reference(instance);
}

inst::instance* builtin_function_caller::double_operator_minus(
  inst::instance& instance, const builtin_function_caller::params_t& params)
{
  const auto lhs = instance.value_cref().get_double();
  const auto& [rhs] = get_params<alternative_t::double_>(params);
  return m_instances.create(lhs - rhs);
}

inst::instance* builtin_function_caller::double_operator_unary_minus(
  inst::instance& instance, const builtin_function_caller::params_t& params)
{
  const auto value = instance.value_cref().get_double();
  return m_instances.create(-1 * value);
}

inst::instance* builtin_function_caller::double_operator_unary_minusminus(
  inst::instance& instance, const builtin_function_caller::params_t& params)
{
  const auto val = instance.value_cref().get_double();
  instance.value_accessor().access().set_double(val - 1.0);
  return m_instances.create_reference(instance);
}

inst::instance* builtin_function_caller::double_operator_star(
  inst::instance& instance, const builtin_function_caller::params_t& params)
{
  const auto lhs = instance.value_cref().get_double();
  const auto& [rhs] = get_params<alternative_t::double_>(params);
  return m_instances.create(lhs * rhs);
}

inst::instance* builtin_function_caller::double_operator_slash(
  inst::instance& instance, const builtin_function_caller::params_t& params)
{
  const auto lhs = instance.value_cref().get_double();
  const auto& [rhs] = get_params<alternative_t::double_>(params);
  return m_instances.create(lhs / rhs);
}

inst::instance* builtin_function_caller::double_operator_equal(
  inst::instance& instance, const builtin_function_caller::params_t& params)
{
  const auto& [rhs] = get_params<alternative_t::double_>(params);
  instance.value_accessor().access().set_double(rhs);
  return m_instances.create_reference(instance);
}

inst::instance* builtin_function_caller::double_operator_plus_equal(
  inst::instance& instance, const builtin_function_caller::params_t& params)
{
  const auto lhs = instance.value_cref().get_double();
  const auto& [rhs] = get_params<alternative_t::double_>(params);

  instance.value_accessor().access().set_double(lhs + rhs);
  return m_instances.create_reference(instance);
}

inst::instance* builtin_function_caller::double_operator_minus_equal(
  inst::instance& instance, const builtin_function_caller::params_t& params)
{
  const auto lhs = instance.value_cref().get_double();
  const auto& [rhs] = get_params<alternative_t::double_>(params);

  instance.value_accessor().access().set_double(lhs - rhs);
  return m_instances.create_reference(instance);
}

inst::instance* builtin_function_caller::double_operator_star_equal(
  inst::instance& instance, const builtin_function_caller::params_t& params)
{
  const auto lhs = instance.value_cref().get_double();
  const auto& [rhs] = get_params<alternative_t::double_>(params);

  instance.value_accessor().access().set_double(lhs * rhs);
  return m_instances.create_reference(instance);
}

inst::instance* builtin_function_caller::double_operator_slash_equal(
  inst::instance& instance, const builtin_function_caller::params_t& params)
{
  const auto lhs = instance.value_cref().get_double();
  const auto& [rhs] = get_params<alternative_t::double_>(params);

  instance.value_accessor().access().set_double(lhs / rhs);
  return m_instances.create_reference(instance);
}

inst::instance* builtin_function_caller::double_operator_less(
  inst::instance& instance, const builtin_function_caller::params_t& params)
{
  const auto lhs = instance.value_cref().get_double();
  const auto& [rhs] = get_params<alternative_t::double_>(params);
  return m_instances.create(lhs < rhs);
}

inst::instance* builtin_function_caller::double_operator_less_equal(
  inst::instance& instance, const builtin_function_caller::params_t& params)
{
  const auto lhs = instance.value_cref().get_double();
  const auto& [rhs] = get_params<alternative_t::double_>(params);
  return m_instances.create(lhs <= rhs);
}

inst::instance* builtin_function_caller::double_operator_greater(
  inst::instance& instance, const builtin_function_caller::params_t& params)
{
  const auto lhs = instance.value_cref().get_double();
  const auto& [rhs] = get_params<alternative_t::double_>(params);
  return m_instances.create(lhs > rhs);
}

inst::instance* builtin_function_caller::double_operator_greater_equal(
  inst::instance& instance, const builtin_function_caller::params_t& params)
{
  const auto lhs = instance.value_cref().get_double();
  const auto& [rhs] = get_params<alternative_t::double_>(params);
  return m_instances.create(lhs >= rhs);
}

inst::instance* builtin_function_caller::string_empty(
  inst::instance& instance, const builtin_function_caller::params_t&)
{
  const auto& str = instance.value_cref().get_string_cref();
  return m_instances.create(str.empty());
}

inst::instance* builtin_function_caller::string_size(
  inst::instance& instance, const builtin_function_caller::params_t&)
{
  const auto& str = instance.value_cref().get_string_cref();
  const auto size = static_cast<int_t>(str.size());
  return m_instances.create(size);
}

inst::instance* builtin_function_caller::string_ctor(
  inst::instance& instance, const builtin_function_caller::params_t&)
{
  instance.value_accessor().access().set_string(std::string{});
  return &instance;
}

inst::instance* builtin_function_caller::string_ctor_string(
  inst::instance& instance, const builtin_function_caller::params_t& params)
{
  const auto& [param] = get_params<alternative_t::string>(params);
  instance.value_accessor().access().set_string(std::move(param));
  return &instance;
}

inst::instance* builtin_function_caller::string_operator_equal_equal(
  inst::instance& instance, const builtin_function_caller::params_t& params)
{
  const auto& lhs = instance.value_cref().get_string_cref();
  const auto& [rhs] = get_params<alternative_t::string>(params);
  return m_instances.create(lhs == rhs);
}

inst::instance* builtin_function_caller::string_operator_not_equal(
  inst::instance& instance, const builtin_function_caller::params_t& params)
{
  const auto& lhs = instance.value_cref().get_string_cref();
  const auto& [rhs] = get_params<alternative_t::string>(params);
  return m_instances.create(lhs != rhs);
}

inst::instance* builtin_function_caller::string_operator_less(
  inst::instance& instance, const builtin_function_caller::params_t& params)
{
  const auto& lhs = instance.value_cref().get_string_cref();
  const auto& [rhs] = get_params<alternative_t::string>(params);
  return m_instances.create(lhs < rhs);
}

inst::instance* builtin_function_caller::string_operator_less_equal(
  inst::instance& instance, const builtin_function_caller::params_t& params)
{
  const auto& lhs = instance.value_cref().get_string_cref();
  const auto& [rhs] = get_params<alternative_t::string>(params);
  return m_instances.create(lhs <= rhs);
}

inst::instance* builtin_function_caller::string_operator_greater(
  inst::instance& instance, const builtin_function_caller::params_t& params)
{
  const auto& lhs = instance.value_cref().get_string_cref();
  const auto& [rhs] = get_params<alternative_t::string>(params);
  return m_instances.create(lhs > rhs);
}

inst::instance* builtin_function_caller::string_operator_greater_equal(
  inst::instance& instance, const builtin_function_caller::params_t& params)
{
  const auto& lhs = instance.value_cref().get_string_cref();
  const auto& [rhs] = get_params<alternative_t::string>(params);
  return m_instances.create(lhs >= rhs);
}

inst::instance* builtin_function_caller::string_operator_plus(
  inst::instance& instance, const builtin_function_caller::params_t& params)
{
  const auto& lhs = instance.value_cref().get_string_cref();
  const auto& [rhs] = get_params<alternative_t::string>(params);
  return m_instances.create(lhs + rhs);
}

inst::instance* builtin_function_caller::string_operator_plus_equal(
  inst::instance& instance, const builtin_function_caller::params_t& params)
{
  auto& lhs = instance.value_accessor().access().get_string_ref();
  const auto& [rhs] = get_params<alternative_t::string>(params);

  lhs += rhs;
  return m_instances.create_reference(instance);
}

inst::instance* builtin_function_caller::string_ctor_string_count(
  inst::instance& instance, const builtin_function_caller::params_t& params)
{
  auto& str = instance.value_accessor().access().get_string_ref();
  const auto& [value, count] =
    get_params<alternative_t::string, alternative_t::int_>(params);

  str.clear();
  str.reserve(value.size() * count);

  for (auto i = 0u; i < count; ++i) {
    str += value;
  }

  return &instance;
}

inst::instance* builtin_function_caller::string_clear(
  inst::instance& instance, const builtin_function_caller::params_t&)
{
  auto& str = instance.value_accessor().access().get_string_ref();
  str.clear();
  return m_instances.create_reference(instance);
}

inst::instance* builtin_function_caller::string_insert_pos_str(
  inst::instance& instance, const builtin_function_caller::params_t& params)
{
  auto& str = instance.value_accessor().access().get_string_ref();
  const auto& [position, value] =
    get_params<alternative_t::int_, alternative_t::string>(params);

  str.insert(position, value);

  return m_instances.create_reference(instance);
}

inst::instance* builtin_function_caller::string_erase_pos(
  inst::instance& instance, const builtin_function_caller::params_t& params)
{
  auto& str = instance.value_accessor().access().get_string_ref();
  const auto& [position] = get_params<alternative_t::int_>(params);
  str.erase(position);
  return m_instances.create_reference(instance);
}

inst::instance* builtin_function_caller::string_erase_pos_count(
  inst::instance& instance, const builtin_function_caller::params_t& params)
{
  auto& str = instance.value_accessor().access().get_string_ref();
  const auto& [position, count] =
    get_params<alternative_t::int_, alternative_t::int_>(params);
  str.erase(position, count);
  return m_instances.create_reference(instance);
}

inst::instance* builtin_function_caller::string_starts_with(
  inst::instance& instance, const builtin_function_caller::params_t& params)
{
  const auto& str = instance.value_cref().get_string_cref();
  const auto& [value] = get_params<alternative_t::string>(params);

  // Todo: extract to common function with ends_with
  const auto starts_with =
    [&str, &value = value] // Workaround for capturing variables introduced by
                           // structured bindings.
  {
    if (&value == &str) {
      return true; // Very same string.
    }
    if (value.length() > str.length()) {
      return false;
    }

    return std::equal(std::cbegin(value), std::cend(value), std::cbegin(str),
                      std::next(std::cbegin(str), value.size()));
  }();

  return m_instances.create(starts_with);
}

inst::instance* builtin_function_caller::string_ends_with(
  inst::instance& instance, const builtin_function_caller::params_t& params)
{
  const auto& str = instance.value_cref().get_string_cref();
  const auto& [value] = get_params<alternative_t::string>(params);

  const auto ends_with =
    [&str, &value = value] // Workaround for capturing variables introduced by
                           // structured bindings.
  {
    if (&value == &str) {
      return true; // Very same string.
    }
    if (value.length() > str.length()) {
      return false;
    }

    return std::equal(std::crbegin(value), std::crend(value),
                      std::crbegin(str),
                      std::next(std::crbegin(str), value.size()));
  }();

  return m_instances.create(ends_with);
}

inst::instance* builtin_function_caller::string_replace_pos_count_str(
  inst::instance& instance, const builtin_function_caller::params_t& params)
{
  auto& str = instance.value_accessor().access().get_string_ref();
  const auto& [position, count, value] =
    get_params<alternative_t::int_, alternative_t::int_,
               alternative_t::string>(params);
  str.replace(position, count, value);
  return m_instances.create_reference(instance);
}

inst::instance* builtin_function_caller::string_substr_pos(
  inst::instance& instance, const builtin_function_caller::params_t& params)
{
  const auto& str = instance.value_cref().get_string_cref();
  const auto& [position] = get_params<alternative_t::int_>(params);
  return m_instances.create(str.substr(position));
}

inst::instance* builtin_function_caller::string_substr_pos_count(
  inst::instance& instance, const builtin_function_caller::params_t& params)
{
  const auto& str = instance.value_cref().get_string_cref();
  const auto& [position, count] =
    get_params<alternative_t::int_, alternative_t::int_>(params);
  return m_instances.create(str.substr(position, count));
}

inst::instance* builtin_function_caller::string_resize_newsize(
  inst::instance& instance, const builtin_function_caller::params_t& params)
{
  auto& str = instance.value_accessor().access().get_string_ref();
  const auto& [new_size] = get_params<alternative_t::int_>(params);
  str.resize(new_size);
  return m_instances.create_reference(instance);
}

inst::instance* builtin_function_caller::string_resize_newsize_fill(
  inst::instance& instance, const builtin_function_caller::params_t& params)
{
  auto& str = instance.value_accessor().access().get_string_ref();
  const auto& [new_size, fill] =
    get_params<alternative_t::int_, alternative_t::string>(params);
  const auto previous_size = str.length();
  str.resize(new_size);

  if (new_size > static_cast<int_t>(previous_size)) {
    const auto difference = new_size - previous_size;
    const auto fill_length = fill.length();
    for (auto i = 0u; i < difference; ++i) {
      str[previous_size + i] = fill[i % fill_length];
    }
  }

  return m_instances.create_reference(instance);
}

inst::instance* builtin_function_caller::string_find_str(
  inst::instance& instance, const builtin_function_caller::params_t& params)
{
  const auto& str = instance.value_cref().get_string_cref();
  const auto& [value] = get_params<alternative_t::string>(params);
  const auto found_pos = str.find(value);
  const auto pos = string_pos_to_int(found_pos);
  return m_instances.create(pos);
}

inst::instance* builtin_function_caller::string_find_str_pos(
  inst::instance& instance, const builtin_function_caller::params_t& params)
{
  const auto& str = instance.value_cref().get_string_cref();
  const auto& [value, start_pos] =
    get_params<alternative_t::string, alternative_t::int_>(params);
  const auto found_pos = str.find(value, start_pos);
  const auto pos = string_pos_to_int(found_pos);
  return m_instances.create(pos);
}

inst::instance* builtin_function_caller::string_find_not_of_str(
  inst::instance& instance, const builtin_function_caller::params_t& params)
{
  const auto& str = instance.value_cref().get_string_cref();
  const auto& [value] = get_params<alternative_t::string>(params);
  const auto found_pos = str.find_first_not_of(value);
  const auto pos = string_pos_to_int(found_pos);
  return m_instances.create(pos);
}

inst::instance* builtin_function_caller::string_find_not_of_str_pos(
  inst::instance& instance, const builtin_function_caller::params_t& params)
{
  const auto& str = instance.value_cref().get_string_cref();
  const auto& [value, start_pos] =
    get_params<alternative_t::string, alternative_t::int_>(params);
  const auto found_pos = str.find_first_not_of(value, start_pos);
  const auto pos = string_pos_to_int(found_pos);
  return m_instances.create(pos);
}

inst::instance* builtin_function_caller::string_find_last_str(
  inst::instance& instance, const builtin_function_caller::params_t& params)
{
  const auto& str = instance.value_cref().get_string_cref();
  const auto& [value] = get_params<alternative_t::string>(params);
  const auto found_pos = str.rfind(value);
  const auto pos = string_pos_to_int(found_pos);
  return m_instances.create(pos);
}

int_t builtin_function_caller::string_pos_to_int(
  std::string::size_type pos) const
{
  return pos != std::string::npos ? static_cast<int_t>(pos) : int_t{ -1 };
}

inst::instance* builtin_function_caller::string_find_last_not_of_str(
  inst::instance& instance, const builtin_function_caller::params_t& params)
{
  const auto& str = instance.value_cref().get_string_cref();
  const auto& [value] = get_params<alternative_t::string>(params);
  const auto found_pos = str.find_last_not_of(value);
  const auto pos = string_pos_to_int(found_pos);
  return m_instances.create(pos);
}

inst::instance* builtin_function_caller::string_contains(
  inst::instance& instance, const builtin_function_caller::params_t& params)
{
  const auto& str = instance.value_cref().get_string_cref();
  const auto& [value] = get_params<alternative_t::string>(params);
  const auto found_pos = str.find(value);
  const auto contains = found_pos != std::string::npos;
  return m_instances.create(contains);
}

inst::instance* builtin_function_caller::string_lower(
  inst::instance& instance, const builtin_function_caller::params_t&)
{
  auto& str = instance.value_accessor().access().get_string_ref();

  for (auto& c : str) {
    c = std::tolower(c);
  }

  return m_instances.create_reference(instance);
}

inst::instance* builtin_function_caller::string_make_lower(
  inst::instance& instance, const builtin_function_caller::params_t&)
{
  const auto& str = instance.value_cref().get_string_cref();
  std::string result;
  result.reserve(str.length());

  std::transform(std::cbegin(str), std::cend(str), std::back_inserter(result),
                 [](const auto c) { return std::tolower(c, m_utf8_locale); });

  return m_instances.create(std::move(result));
}

inst::instance* builtin_function_caller::string_upper(
  inst::instance& instance, const builtin_function_caller::params_t&)
{
  auto& str = instance.value_accessor().access().get_string_ref();

  for (auto& c : str) {
    c = std::toupper(c);
  }

  return m_instances.create_reference(instance);
}

inst::instance* builtin_function_caller::string_make_upper(
  inst::instance& instance, const builtin_function_caller::params_t&)
{
  const auto& str = instance.value_cref().get_string_cref();
  std::string result;
  result.reserve(str.length());

  std::transform(std::cbegin(str), std::cend(str), std::back_inserter(result),
                 [](const auto c) { return std::toupper(c, m_utf8_locale); });

  return m_instances.create(std::move(result));
}

inst::instance* builtin_function_caller::version_ctor_major(
  inst::instance& instance, const builtin_function_caller::params_t& params)
{
  const auto& [major] = get_params<alternative_t::int_>(params);
  instance.value_accessor().access().set_version(inst::version_value{ major });
  return &instance;
}

inst::instance* builtin_function_caller::version_ctor_major_minor(
  inst::instance& instance, const builtin_function_caller::params_t& params)
{
  const auto& [major, minor] =
    get_params<alternative_t::int_, alternative_t::int_>(params);
  instance.value_accessor().access().set_version(
    inst::version_value{ major, minor });
  return &instance;
}

inst::instance* builtin_function_caller::version_ctor_major_minor_patch(
  inst::instance& instance, const builtin_function_caller::params_t& params)
{
  const auto& [major, minor, patch] =
    get_params<alternative_t::int_, alternative_t::int_, alternative_t::int_>(
      params);
  instance.value_accessor().access().set_version(
    inst::version_value{ major, minor, patch });
  return &instance;
}

inst::instance* builtin_function_caller::version_ctor_major_minor_patch_tweak(
  inst::instance& instance, const builtin_function_caller::params_t& params)
{
  const auto& [major, minor, patch, tweak] =
    get_params<alternative_t::int_, alternative_t::int_, alternative_t::int_,
               alternative_t::int_>(params);
  instance.value_accessor().access().set_version(
    inst::version_value{ major, minor, patch, tweak });
  return &instance;
}

inst::instance* builtin_function_caller::version_operator_equal_equal(
  inst::instance& instance, const builtin_function_caller::params_t& params)
{
  const auto& ver = instance.value_cref().get_version_cref();
  const auto& [ver2] = get_params<alternative_t::version>(params);
  return m_instances.create(ver == ver2);
}

inst::instance* builtin_function_caller::version_operator_not_equal(
  inst::instance& instance, const builtin_function_caller::params_t& params)
{
  const auto& ver = instance.value_cref().get_version_cref();
  const auto& [ver2] = get_params<alternative_t::version>(params);
  return m_instances.create(ver != ver2);
}

inst::instance* builtin_function_caller::version_operator_less(
  inst::instance& instance, const builtin_function_caller::params_t& params)
{
  const auto& ver = instance.value_cref().get_version_cref();
  const auto& [ver2] = get_params<alternative_t::version>(params);
  return m_instances.create(ver < ver2);
}

inst::instance* builtin_function_caller::version_operator_less_equal(
  inst::instance& instance, const builtin_function_caller::params_t& params)
{
  const auto& ver = instance.value_cref().get_version_cref();
  const auto& [ver2] = get_params<alternative_t::version>(params);
  return m_instances.create(ver <= ver2);
}

inst::instance* builtin_function_caller::version_operator_greater(
  inst::instance& instance, const builtin_function_caller::params_t& params)
{
  const auto& ver = instance.value_cref().get_version_cref();
  const auto& [ver2] = get_params<alternative_t::version>(params);
  return m_instances.create(ver > ver2);
}

inst::instance* builtin_function_caller::version_operator_greater_equal(
  inst::instance& instance, const builtin_function_caller::params_t& params)
{
  const auto& ver = instance.value_cref().get_version_cref();
  const auto& [ver2] = get_params<alternative_t::version>(params);
  return m_instances.create(ver >= ver2);
}

inst::instance* builtin_function_caller::version_major(
  inst::instance& instance, const builtin_function_caller::params_t&)
{
  const auto& ver = instance.value_cref().get_version_cref();
  return m_instances.create(ver.major_());
}

inst::instance* builtin_function_caller::version_minor(
  inst::instance& instance, const builtin_function_caller::params_t&)
{
  const auto& ver = instance.value_cref().get_version_cref();
  return m_instances.create(ver.minor_());
}

inst::instance* builtin_function_caller::version_patch(
  inst::instance& instance, const builtin_function_caller::params_t&)
{
  const auto& ver = instance.value_cref().get_version_cref();
  return m_instances.create(ver.patch());
}

inst::instance* builtin_function_caller::version_tweak(
  inst::instance& instance, const builtin_function_caller::params_t&)
{
  const auto& ver = instance.value_cref().get_version_cref();
  return m_instances.create(ver.tweak());
}

inst::instance* builtin_function_caller::version_to_string(
  inst::instance& instance, const builtin_function_caller::params_t&)
{
  const auto& ver = instance.value_cref().get_version_cref();
  return m_instances.create(ver.to_string());
}

inst::instance* builtin_function_caller::extern_constructor_name(
  inst::instance& instance, const builtin_function_caller::params_t& params)
{
  const auto& [name] = get_params<alternative_t ::string>(params);
  const auto got_value = m_cmake_facade.try_get_extern_define(name);

  std::unique_ptr<inst::instance> extern_instance;

  if (got_value) {
    extern_argument_parser parser;

    const auto& ty =
      static_cast<const sema::homogeneous_generic_type&>(instance.type());
    const auto& expected_type = ty.value_type();
    if (expected_type == m_builtin_types.bool_) {
      const auto parsed = parser.try_bool(*got_value);
      if (parsed) {
        const auto created = m_instances.create(*parsed);
        extern_instance = m_instances.gather_ownership(created);
      }
    } else if (expected_type == m_builtin_types.int_) {
      const auto parsed = parser.try_int(*got_value);
      if (parsed) {
        const auto created = m_instances.create(*parsed);
        extern_instance = m_instances.gather_ownership(created);
      }
    } else if (expected_type == m_builtin_types.double_) {
      const auto parsed = parser.try_double(*got_value);
      if (parsed) {
        const auto created = m_instances.create(*parsed);
        extern_instance = m_instances.gather_ownership(created);
      }
    } else if (expected_type == m_builtin_types.string) {
      const auto parsed = parser.try_string(*got_value);
      if (parsed) {
        const auto created = m_instances.create(*parsed);
        extern_instance = m_instances.gather_ownership(created);
      }
    }
  }

  instance.value_accessor().access().set_extern(
    inst::extern_value{ std::move(extern_instance) });
  return &instance;
}

inst::instance* builtin_function_caller::extern_has_value(
  inst::instance& instance, const builtin_function_caller::params_t&)
{
  const auto& extern_val = instance.value_cref().get_extern_cref();
  const auto has_value = extern_val.has_value();
  return m_instances.create(has_value);
}

inst::instance* builtin_function_caller::extern_value(
  inst::instance& instance, const builtin_function_caller::params_t&)
{
  const auto& extern_val = instance.value_cref().get_extern_cref();
  auto value_copy = extern_val.value_cref().copy();
  auto ptr = value_copy.get();
  m_instances.store(std::move(value_copy));
  return ptr;
}

inst::instance* builtin_function_caller::list_ctor(
  inst::instance& instance, const builtin_function_caller::params_t&)
{
  auto& list = instance.value_accessor().access().get_list_ref();
  list = inst::list_value{};
  return m_instances.create_reference(instance);
}

inst::instance* builtin_function_caller::list_size(
  inst::instance& instance, const builtin_function_caller::params_t&)
{
  const auto& list = instance.value_cref().get_list_cref();
  return m_instances.create(static_cast<int_t>(list.size()));
}

inst::instance* builtin_function_caller::list_ctor_list(
  inst::instance& instance, const builtin_function_caller::params_t& params)
{
  auto& list = instance.value_accessor().access().get_list_ref();
  const auto& [value] = get_params<alternative_t::list>(params);
  list = inst::list_value{ value };
  return m_instances.create_reference(instance);
}

inst::instance* builtin_function_caller::list_ctor_value(
  inst::instance& instance, const builtin_function_caller::params_t& params)
{
  auto& list = instance.value_accessor().access().get_list_ref();
  list.push_back(params[0]->copy());
  return m_instances.create_reference(instance);
}

inst::instance* builtin_function_caller::list_ctor_value_count(
  inst::instance& instance, const builtin_function_caller::params_t& params)
{
  auto& list = instance.value_accessor().access().get_list_ref();
  const auto count = params[1]->value_cref().get_int();

  for (auto i = 0u; i < count; ++i) {
    list.push_back(params[0]->copy());
  }

  return m_instances.create_reference(instance);
}

inst::instance* builtin_function_caller::list_push_back_value(
  inst::instance& instance, const builtin_function_caller::params_t& params)
{
  auto& list = instance.value_accessor().access().get_list_ref();
  list.push_back(params[0]->copy());
  return m_instances.create_void();
}

inst::instance* builtin_function_caller::list_push_back_list(
  inst::instance& instance, const builtin_function_caller::params_t& params)
{
  auto& list = instance.value_accessor().access().get_list_ref();
  const auto& [list2] = get_params<alternative_t::list>(params);
  list.push_back(list2);
  return m_instances.create_void();
}

inst::instance* builtin_function_caller::list_push_front_value(
  inst::instance& instance, const builtin_function_caller::params_t& params)
{
  auto& list = instance.value_accessor().access().get_list_ref();
  list.push_front(params[0]->copy());
  return m_instances.create_void();
}

inst::instance* builtin_function_caller::list_push_front_list(
  inst::instance& instance, const builtin_function_caller::params_t& params)
{
  auto& list = instance.value_accessor().access().get_list_ref();
  const auto& [list2] = get_params<alternative_t::list>(params);
  list.push_front(list2);
  return m_instances.create_void();
}

inst::instance* builtin_function_caller::list_pop_back(
  inst::instance& instance, const builtin_function_caller::params_t&)
{
  auto& list = instance.value_accessor().access().get_list_ref();
  list.pop_back();
  return m_instances.create_void();
}

inst::instance* builtin_function_caller::list_pop_front(
  inst::instance& instance, const builtin_function_caller::params_t&)
{
  auto& list = instance.value_accessor().access().get_list_ref();
  list.pop_front();
  return m_instances.create_void();
}

inst::instance* builtin_function_caller::list_at(
  inst::instance& instance, const builtin_function_caller::params_t& params)
{
  auto& list = instance.value_accessor().access().get_list_ref();
  const auto& [position] = get_params<alternative_t::int_>(params);
  auto& instance_at = list.at(position);
  return m_instances.create_reference(instance_at);
}

inst::instance* builtin_function_caller::list_front(
  inst::instance& instance, const builtin_function_caller::params_t&)
{
  auto& list = instance.value_accessor().access().get_list_ref();
  auto& instance_at_front = list.front();
  return m_instances.create_reference(instance_at_front);
}

inst::instance* builtin_function_caller::list_back(
  inst::instance& instance, const builtin_function_caller::params_t&)
{
  auto& list = instance.value_accessor().access().get_list_ref();
  auto& instance_at_back = list.back();
  return m_instances.create_reference(instance_at_back);
}

inst::instance* builtin_function_caller::list_insert_pos_value(
  inst::instance& instance, const builtin_function_caller::params_t& params)
{
  auto& list = instance.value_accessor().access().get_list_ref();
  const auto& [position] = get_params<alternative_t::int_>(params);
  list.insert(position, params[1]->copy());
  return m_instances.create_void();
}

inst::instance* builtin_function_caller::list_insert_pos_list(
  inst::instance& instance, const builtin_function_caller::params_t& params)
{
  auto& list = instance.value_accessor().access().get_list_ref();
  const auto& [position, list2] =
    get_params<alternative_t::int_, alternative_t::list>(params);
  list.insert(position, list2);
  return m_instances.create_void();
}

inst::instance* builtin_function_caller::list_erase_pos(
  inst::instance& instance, const builtin_function_caller::params_t& params)
{
  auto& list = instance.value_accessor().access().get_list_ref();
  const auto& [position] = get_params<alternative_t::int_>(params);
  list.erase(position);
  return m_instances.create_void();
}

inst::instance* builtin_function_caller::list_erase_pos_count(
  inst::instance& instance, const builtin_function_caller::params_t& params)
{
  auto& list = instance.value_accessor().access().get_list_ref();
  const auto& [position, count] =
    get_params<alternative_t::int_, alternative_t::int_>(params);
  list.erase(position, count);
  return m_instances.create_void();
}

inst::instance* builtin_function_caller::list_remove_value(
  inst::instance& instance, const builtin_function_caller::params_t& params)
{
  auto& list = instance.value_accessor().access().get_list_ref();
  const auto& value = *params[0];
  const auto removed_count = list.remove(value);
  return m_instances.create(removed_count);
}

inst::instance* builtin_function_caller::list_remove_value_count(
  inst::instance& instance, const builtin_function_caller::params_t& params)
{
  auto& list = instance.value_accessor().access().get_list_ref();
  const auto& value = *params[0];
  const auto& count = params[1]->value_cref().get_int();
  const auto removed_count = list.remove(value, count);
  return m_instances.create(removed_count);
}

inst::instance* builtin_function_caller::list_remove_last_value_count(
  inst::instance& instance, const builtin_function_caller::params_t& params)
{
  auto& list = instance.value_accessor().access().get_list_ref();
  const auto& value = *params[0];
  const auto& count = params[1]->value_cref().get_int();
  const auto removed_count = list.remove_last(value, count);
  return m_instances.create(removed_count);
}

inst::instance* builtin_function_caller::list_clear(
  inst::instance& instance, const builtin_function_caller::params_t&)
{
  auto& list = instance.value_accessor().access().get_list_ref();
  list.clear();
  return m_instances.create_void();
}

inst::instance* builtin_function_caller::list_resize(
  inst::instance& instance, const builtin_function_caller::params_t& params)
{
  const auto& list_type =
    *dynamic_cast<const sema::homogeneous_generic_type*>(&instance.type());
  const auto& value_type = list_type.value_type();

  auto& list = instance.value_accessor().access().get_list_ref();
  const auto& [new_size] = get_params<alternative_t::int_>(params);

  const auto fill_value =
    new_size > list.size() ? m_instances.create(value_type) : nullptr;
  list.resize(new_size, fill_value);
  return m_instances.create_void();
}

inst::instance* builtin_function_caller::list_sort(
  inst::instance& instance, const builtin_function_caller::params_t&)
{
  auto& list = instance.value_accessor().access().get_list_ref();
  list.sort();
  return m_instances.create_void();
}

inst::instance* builtin_function_caller::list_reverse(
  inst::instance& instance, const builtin_function_caller::params_t&)
{
  auto& list = instance.value_accessor().access().get_list_ref();
  list.reverse();
  return m_instances.create_void();
}

inst::instance* builtin_function_caller::list_min(
  inst::instance& instance, const builtin_function_caller::params_t&)
{
  const auto& list = instance.value_cref().get_list_cref();
  const auto min_element_index = list.min();
  return m_instances.create(min_element_index);
}

inst::instance* builtin_function_caller::list_max(
  inst::instance& instance, const builtin_function_caller::params_t&)
{
  const auto& list = instance.value_cref().get_list_cref();
  const auto max_element_index = list.max();
  return m_instances.create(max_element_index);
}

inst::instance* builtin_function_caller::list_sublist_pos(
  inst::instance& instance, const builtin_function_caller::params_t& params)
{
  const auto& list = instance.value_cref().get_list_cref();
  const auto& [position] = get_params<alternative_t::int_>(params);
  auto sublist = list.sublist(position);
  return m_instances.create(instance.type(), std::move(sublist));
}

inst::instance* builtin_function_caller::list_sublist_pos_count(
  inst::instance& instance, const builtin_function_caller::params_t& params)
{
  const auto& list = instance.value_cref().get_list_cref();
  const auto& [position, count] =
    get_params<alternative_t::int_, alternative_t::int_>(params);
  auto sublist = list.sublist(position, count);
  return m_instances.create(instance.type(), std::move(sublist));
}

inst::instance* builtin_function_caller::list_empty(
  inst::instance& instance, const builtin_function_caller::params_t&)
{
  const auto& list = instance.value_cref().get_list_cref();
  const auto is_empty = list.empty();
  return m_instances.create(is_empty);
}

inst::instance* builtin_function_caller::list_find_value(
  inst::instance& instance, const builtin_function_caller::params_t& params)
{
  const auto& list = instance.value_cref().get_list_cref();
  const auto& value = *params[0];
  const auto found_index = list.find(value);
  return m_instances.create(found_index);
}

inst::instance* builtin_function_caller::list_find_value_pos(
  inst::instance& instance, const builtin_function_caller::params_t& params)
{
  const auto& list = instance.value_cref().get_list_cref();
  const auto& value = *params[0];
  const auto position = params[1]->value_cref().get_int();
  const auto found_index = list.find(value, position);
  return m_instances.create(found_index);
}

inst::instance* builtin_function_caller::list_operator_plus_value(
  inst::instance& instance, const builtin_function_caller::params_t& params)
{
  auto list_copy = instance.value_cref().get_list_cref();
  auto value = params[0]->copy();
  list_copy.push_back(std::move(value));
  return m_instances.create(instance.type(), std::move(list_copy));
}

inst::instance* builtin_function_caller::list_operator_plus_list(
  inst::instance& instance, const builtin_function_caller::params_t& params)
{
  auto list_copy = instance.value_cref().get_list_cref();
  auto [list_to_append] = get_params<alternative_t::list>(params);
  list_copy.push_back(std::move(list_to_append));
  return m_instances.create(instance.type(), std::move(list_copy));
}

inst::instance* builtin_function_caller::list_operator_plus_equal_value(
  inst::instance& instance, const builtin_function_caller::params_t& params)
{
  auto& list = instance.value_accessor().access().get_list_ref();
  auto value = params[0]->copy();
  list.push_back(std::move(value));
  return m_instances.create_reference(instance);
}

inst::instance* builtin_function_caller::list_operator_plus_equal_list(
  inst::instance& instance, const builtin_function_caller::params_t& params)
{
  auto& list = instance.value_accessor().access().get_list_ref();
  auto [list_to_append] = get_params<alternative_t::list>(params);
  list.push_back(std::move(list_to_append));
  return m_instances.create_reference(instance);
}

inst::instance* builtin_function_caller::cmake_minimum_required(
  const builtin_function_caller::params_t& params)
{
  const auto got_version = m_cmake_facade.get_cmake_version();
  const inst::version_value current_version{
    static_cast<int_t>(got_version.major),
    static_cast<int_t>(got_version.minor),
    static_cast<int_t>(got_version.patch),
    static_cast<int_t>(got_version.tweak)
  };

  const auto& [requested_version] = get_params<alternative_t::version>(params);
  const auto version_satisfied = requested_version <= current_version;

  if (!version_satisfied) {
    m_cmake_facade.fatal_error("Running on a older version that is required.");
    // Todo: Propagate error to our execution system, so it knows it needs to
    // stop execution.
  }

  return m_instances.create_void();
}

inst::instance* builtin_function_caller::project_ctor_name(
  inst::instance& instance, const builtin_function_caller::params_t& params)
{
  auto& project = instance.value_accessor().access().get_project_ref();
  const auto& [name] = get_params<alternative_t::string>(params);
  project = inst::project_value{ name };
  m_cmake_facade.register_project(name);
  return &instance;
}

inst::instance* builtin_function_caller::project_name(
  inst::instance& instance, const builtin_function_caller::params_t&)
{
  const auto& project = instance.value_cref().get_project_cref();
  return m_instances.create(project.name());
}

inst::instance* builtin_function_caller::project_add_executable(
  inst::instance& instance, const builtin_function_caller::params_t& params)
{
  auto& project = instance.value_accessor().access().get_project_ref();
  const auto& [name, sources] =
    get_params<alternative_t::string, alternative_t::list>(params);
  project.add_executable(m_cmake_facade, name, sources);
  return m_instances.create(inst::executable_value{ name });
}

inst::instance* builtin_function_caller::project_add_library(
  inst::instance& instance, const builtin_function_caller::params_t& params)
{
  auto& project = instance.value_accessor().access().get_project_ref();
  const auto& [name, sources] =
    get_params<alternative_t::string, alternative_t::list>(params);
  project.add_library(m_cmake_facade, name, sources);
  return m_instances.create(inst::library_value{ name });
}

inst::instance* builtin_function_caller::project_find_library(
  inst::instance& instance, const builtin_function_caller::params_t& params)
{
  const auto& [name] = get_params<alternative_t::string>(params);
  return m_instances.create(inst::library_value{ name });
}

inst::instance* builtin_function_caller::library_name(
  inst::instance& instance, const builtin_function_caller::params_t&)
{
  const auto& target = instance.value_cref().get_library_cref();
  return m_instances.create(target.name());
}

inst::instance* builtin_function_caller::library_link_to(
  inst::instance& instance, const builtin_function_caller::params_t& params)
{
  const auto& target = instance.value_cref().get_library_cref();
  const auto& [library] = get_params<alternative_t::library>(params);
  const auto default_visibility_value = facade::visibility::private_;
  target.link_to(m_cmake_facade, default_visibility_value, library);
  return m_instances.create_void();
}

inst::instance* builtin_function_caller::library_link_to_visibility(
  inst::instance& instance, const builtin_function_caller::params_t& params)
{
  const auto& target = instance.value_cref().get_library_cref();
  const auto& [library, visibility] =
    get_params<alternative_t::library, alternative_t::enum_>(params);
  const auto facade_visibility_value =
    static_cast<facade::visibility>(visibility.value);
  target.link_to(m_cmake_facade, facade_visibility_value, library);
  return m_instances.create_void();
}

inst::instance* builtin_function_caller::library_include_directories(
  inst::instance& instance, const builtin_function_caller::params_t& params)
{
  const auto& [dirs] = get_params<alternative_t::list>(params);
  const auto& lib = instance.value_cref().get_library_cref();
  const auto default_visibility_value = facade::visibility::private_;
  lib.include_directories(m_cmake_facade, default_visibility_value, dirs);
  return m_instances.create_void();
}

inst::instance*
builtin_function_caller::library_include_directories_visibility(
  inst::instance& instance, const builtin_function_caller::params_t& params)
{
  const auto& [dirs, visibility] =
    get_params<alternative_t::list, alternative_t::enum_>(params);
  const auto& lib = instance.value_cref().get_library_cref();
  const auto facade_visibility_value =
    static_cast<facade::visibility>(visibility.value);
  lib.include_directories(m_cmake_facade, facade_visibility_value, dirs);
  return m_instances.create_void();
}

inst::instance* builtin_function_caller::executable_name(
  inst::instance& instance, const builtin_function_caller::params_t&)
{
  const auto& target = instance.value_cref().get_executable_cref();
  return m_instances.create(target.name());
}

inst::instance* builtin_function_caller::executable_link_to(
  inst::instance& instance, const builtin_function_caller::params_t& params)
{
  const auto& target = instance.value_cref().get_executable_cref();
  const auto& [library] = get_params<alternative_t::library>(params);
  const auto defalult_visibility_value = facade::visibility::private_;
  target.link_to(m_cmake_facade, defalult_visibility_value, library);
  return m_instances.create_void();
}

inst::instance* builtin_function_caller::executable_link_to_visibility(
  inst::instance& instance, const builtin_function_caller::params_t& params)
{
  const auto& target = instance.value_cref().get_executable_cref();
  const auto& [library, visibility] =
    get_params<alternative_t::library, alternative_t::enum_>(params);
  const auto facade_visibility_value =
    static_cast<facade::visibility>(visibility.value);
  target.link_to(m_cmake_facade, facade_visibility_value, library);
  return m_instances.create_void();
}

inst::instance* builtin_function_caller::executable_include_directories(
  inst::instance& instance, const builtin_function_caller::params_t& params)
{
  const auto& [dirs] = get_params<alternative_t::list>(params);
  const auto& exe = instance.value_cref().get_executable_cref();
  const auto default_visibility_value = facade::visibility::private_;
  exe.include_directories(m_cmake_facade, default_visibility_value, dirs);
  return m_instances.create_void();
}

inst::instance*
builtin_function_caller::executable_include_directories_visibility(
  inst::instance& instance, const builtin_function_caller::params_t& params)
{
  const auto& [dirs, visibility] =
    get_params<alternative_t::list, alternative_t::enum_>(params);
  const auto facade_visibility_value =
    static_cast<facade::visibility>(visibility.value);
  const auto& exe = instance.value_cref().get_executable_cref();
  exe.include_directories(m_cmake_facade, facade_visibility_value, dirs);
  return m_instances.create_void();
}

inst::instance* builtin_function_caller::enum_to_string(
  inst::instance& instance, const builtin_function_caller::params_t& params)
{
  const auto& enum_type = static_cast<const sema::enum_type&>(instance.type());
  const auto enum_value = instance.value_cref().get_enum_constant();
  const auto enum_value_name = enum_type.enumerator(enum_value.value).str();
  return m_instances.create(std::string{ enum_value_name });
}

inst::instance* builtin_function_caller::enum_operator_equal(
  inst::instance& instance, const builtin_function_caller::params_t& params)
{
  const auto& [enum_value] = get_params<alternative_t::enum_>(params);
  instance.value_accessor().access().set_enum_constant(enum_value);
  return m_instances.create_reference(instance);
}

inst::instance* builtin_function_caller::enum_operator_equalequal(
  inst::instance& instance, const builtin_function_caller::params_t& params)
{
  const auto& [rhs] = get_params<alternative_t::enum_>(params);
  const auto& lhs = instance.value_cref().get_enum_constant();
  const auto result = (lhs.value == rhs.value);
  return m_instances.create(result);
}

inst::instance* builtin_function_caller::enum_operator_exclaimequal(
  inst::instance& instance, const builtin_function_caller::params_t& params)
{
  const auto& [rhs] = get_params<alternative_t::enum_>(params);
  const auto& lhs = instance.value_cref().get_enum_constant();
  const auto result = (lhs.value != rhs.value);
  return m_instances.create(result);
}

inst::instance* builtin_function_caller::user_type_operator_equal(
  inst::instance& instance, const builtin_function_caller::params_t& params)
{
  instance.assign(params[0]->copy());
  return m_instances.create_reference(instance);
}

inst::instance* builtin_function_caller::option_ctor_description(
  inst::instance& instance, const builtin_function_caller::params_t& params)
{
  const auto& [name, description] =
    get_params<alternative_t::string, alternative_t::string>(params);
  const auto existing_value = m_cmake_facade.get_option_value(name);
  bool value = false;
  if (existing_value) {
    value = *existing_value;
  } else {
    m_cmake_facade.register_option(name, description, value);
  }

  instance.value_accessor().access().set_option(
    inst::option_value{ description, value });
  return &instance;
}

inst::instance* builtin_function_caller::option_ctor_description_value(
  inst::instance& instance, const builtin_function_caller::params_t& params)
{
  const auto& [name, description, param_value] =
    get_params<alternative_t::string, alternative_t::string,
               alternative_t::bool_>(params);
  const auto existing_value = m_cmake_facade.get_option_value(name);
  bool value = param_value;
  if (existing_value) {
    value = *existing_value;
  } else {
    m_cmake_facade.register_option(name, description, value);
  }

  instance.value_accessor().access().set_option(
    inst::option_value{ description, value });
  return &instance;
}

inst::instance* builtin_function_caller::option_value(
  inst::instance& instance, const builtin_function_caller::params_t& params)
{
  const auto value = instance.value_cref().get_option_cref().value();
  return m_instances.create(value);
}

inst::instance* builtin_function_caller::cmake_message(
  const builtin_function_caller::params_t& params)
{
  const auto& [message] = get_params<alternative_t::string>(params);
  m_cmake_facade.message(message);
  return m_instances.create_void();
}

inst::instance* builtin_function_caller::cmake_warning(
  const builtin_function_caller::params_t& params)
{
  const auto& [message] = get_params<alternative_t::string>(params);
  m_cmake_facade.warning(message);
  return m_instances.create_void();
}

inst::instance* builtin_function_caller::cmake_error(
  const builtin_function_caller::params_t& params)
{
  const auto& [message] = get_params<alternative_t::string>(params);
  m_cmake_facade.error(message);
  return m_instances.create_void();
}

inst::instance* builtin_function_caller::cmake_fatal_error(
  const builtin_function_caller::params_t& params)
{
  const auto& [message] = get_params<alternative_t::string>(params);
  m_cmake_facade.fatal_error(message);
  return m_instances.create_void();
}

inst::instance* builtin_function_caller::cmake_get_cxx_compiler_info(
  const builtin_function_caller::params_t& params)
{
  auto created_variable =
    m_instances.create(m_builtin_types.cmake->cxx_compiler_info);
  auto id_member_info = created_variable->type().find_member("id");
  auto id_member = created_variable->find_member(id_member_info->index);

  const auto info = m_cmake_facade.get_cxx_compiler_info();

  const auto enum_val =
    inst::enum_constant_value{ static_cast<unsigned>(info.id) };
  id_member->value_accessor().access().set_enum_constant(enum_val);

  return created_variable;
}

inst::instance* builtin_function_caller::cmake_install_executable(
  const builtin_function_caller::params_t& params)
{
  const auto& [exe] = get_params<alternative_t::executable>(params);
  m_cmake_facade.install(exe.name());
  return m_instances.create_void();
}

inst::instance* builtin_function_caller::cmake_install_library(
  const builtin_function_caller::params_t& params)
{
  const auto& [lib] = get_params<alternative_t::library>(params);
  m_cmake_facade.install(lib.name());
  return m_instances.create_void();
}

inst::instance* builtin_function_caller::cmake_enable_ctest(
  const builtin_function_caller::params_t&)
{
  m_cmake_facade.enable_ctest();
  return m_instances.create_void();
}

inst::instance* builtin_function_caller::cmake_add_test(
  const builtin_function_caller::params_t& params)
{
  const auto& [exe] = get_params<alternative_t::executable>(params);
  m_cmake_facade.add_test(exe.name());
  return m_instances.create_void();
}

inst::instance* builtin_function_caller::cmake_root_source_dir(
  const builtin_function_caller::params_t& params)
{
  auto dir = m_cmake_facade.get_root_source_dir();
  return m_instances.create(std::move(dir));
}

inst::instance* builtin_function_caller::cmake_add_custom_command(
  const builtin_function_caller::params_t& params)
{
  const auto& [command_list, output] =
    get_params<alternative_t::list, alternative_t ::string>(params);
  const auto command = inst::list_value_utils{ command_list }.strings();
  m_cmake_facade.add_custom_command(command, output);
  return m_instances.create_void();
}

inst::instance* builtin_function_caller::cmake_current_binary_dir(
  const builtin_function_caller::params_t& params)
{
  auto dir = m_cmake_facade.get_current_binary_dir();
  return m_instances.create(std::move(dir));
}

inst::instance* builtin_function_caller::cmake_make_directory(
  const builtin_function_caller::params_t& params)
{
  const auto& [dir] = get_params<alternative_t ::string>(params);
  m_cmake_facade.make_directory(dir);
  return m_instances.create_void();
}

inst::instance* builtin_function_caller::cmake_set_old_style_variable(
  const builtin_function_caller::params_t& params)
{
  const auto& [name, value] =
    get_params<alternative_t ::string, alternative_t ::string>(params);
  m_cmake_facade.set_old_style_variable(name, value);
  return m_instances.create_void();
}
}
