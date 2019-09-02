#pragma once

#include "common/int_alias.hpp"

#include <locale>
#include <memory>
#include <vector>

namespace cmsl {
namespace facade {
class cmake_facade;
}

namespace sema {
enum class builtin_function_kind;
struct builtin_types_accessor;
}

namespace exec {
namespace inst {
class instance;
class instances_holder_interface;
}

class builtin_function_caller
{
public:
  using params_t = std::vector<inst::instance*>;

  explicit builtin_function_caller(
    facade::cmake_facade& cmake_facade,
    inst::instances_holder_interface& instances,
    const sema::builtin_types_accessor& builtin_types);

  std::unique_ptr<inst::instance> call(
    sema::builtin_function_kind function_kind, const params_t& params);
  std::unique_ptr<inst::instance> call_member(
    inst::instance& instance, sema::builtin_function_kind function_kind,
    const params_t& params);

private:
  inst::instance* bool_ctor(inst::instance& instance, const params_t& params);
  inst::instance* bool_ctor_bool(inst::instance& instance,
                                 const params_t& params);
  inst::instance* bool_ctor_int(inst::instance& instance,
                                const params_t& params);
  inst::instance* bool_operator_equal(inst::instance& instance,
                                      const params_t& params);
  inst::instance* bool_operator_equal_equal(inst::instance& instance,
                                            const params_t& params);
  inst::instance* bool_operator_pipe_pipe(inst::instance& instance,
                                          const params_t& params);
  inst::instance* bool_operator_amp_amp(inst::instance& instance,
                                        const params_t& params);
  inst::instance* bool_operator_unary_exclaim(inst::instance& instance,
                                              const params_t& params);
  inst::instance* bool_to_string(inst::instance& instance,
                                 const params_t& params);

  inst::instance* int_ctor(inst::instance& instance, const params_t& params);
  inst::instance* int_ctor_bool(inst::instance& instance,
                                const params_t& params);
  inst::instance* int_ctor_int(inst::instance& instance,
                               const params_t& params);
  inst::instance* int_ctor_double(inst::instance& instance,
                                  const params_t& params);
  inst::instance* int_operator_plus(inst::instance& instance,
                                    const params_t& params);
  inst::instance* int_operator_unary_plusplus(inst::instance& instance,
                                              const params_t& params);
  inst::instance* int_operator_minus(inst::instance& instance,
                                     const params_t& params);
  inst::instance* int_operator_unary_minus(inst::instance& instance,
                                           const params_t& params);
  inst::instance* int_operator_unary_minusminus(inst::instance& instance,
                                                const params_t& params);
  inst::instance* int_operator_star(inst::instance& instance,
                                    const params_t& params);
  inst::instance* int_operator_slash(inst::instance& instance,
                                     const params_t& params);
  inst::instance* int_operator_equal(inst::instance& instance,
                                     const params_t& params);
  inst::instance* int_operator_plus_equal(inst::instance& instance,
                                          const params_t& params);
  inst::instance* int_operator_minus_equal(inst::instance& instance,
                                           const params_t& params);
  inst::instance* int_operator_star_equal(inst::instance& instance,
                                          const params_t& params);
  inst::instance* int_operator_slash_equal(inst::instance& instance,
                                           const params_t& params);
  inst::instance* int_operator_less(inst::instance& instance,
                                    const params_t& params);
  inst::instance* int_operator_less_equal(inst::instance& instance,
                                          const params_t& params);
  inst::instance* int_operator_greater(inst::instance& instance,
                                       const params_t& params);
  inst::instance* int_operator_greater_equal(inst::instance& instance,
                                             const params_t& params);
  inst::instance* int_operator_equal_equal(inst::instance& instance,
                                           const params_t& params);
  inst::instance* int_to_string(inst::instance& instance,
                                const params_t& params);

  inst::instance* double_ctor(inst::instance& instance,
                              const params_t& params);
  inst::instance* double_ctor_double(inst::instance& instance,
                                     const params_t& params);
  inst::instance* double_ctor_int(inst::instance& instance,
                                  const params_t& params);
  inst::instance* double_operator_plus(inst::instance& instance,
                                       const params_t& params);
  inst::instance* double_operator_unary_plusplus(inst::instance& instance,
                                                 const params_t& params);
  inst::instance* double_operator_minus(inst::instance& instance,
                                        const params_t& params);
  inst::instance* double_operator_unary_minus(inst::instance& instance,
                                              const params_t& params);
  inst::instance* double_operator_unary_minusminus(inst::instance& instance,
                                                   const params_t& params);
  inst::instance* double_operator_star(inst::instance& instance,
                                       const params_t& params);
  inst::instance* double_operator_slash(inst::instance& instance,
                                        const params_t& params);
  inst::instance* double_operator_equal(inst::instance& instance,
                                        const params_t& params);
  inst::instance* double_operator_plus_equal(inst::instance& instance,
                                             const params_t& params);
  inst::instance* double_operator_minus_equal(inst::instance& instance,
                                              const params_t& params);
  inst::instance* double_operator_star_equal(inst::instance& instance,
                                             const params_t& params);
  inst::instance* double_operator_slash_equal(inst::instance& instance,
                                              const params_t& params);
  inst::instance* double_operator_less(inst::instance& instance,
                                       const params_t& params);
  inst::instance* double_operator_less_equal(inst::instance& instance,
                                             const params_t& params);
  inst::instance* double_operator_greater(inst::instance& instance,
                                          const params_t& params);
  inst::instance* double_operator_greater_equal(inst::instance& instance,
                                                const params_t& params);

  inst::instance* string_ctor(inst::instance& instance,
                              const params_t& params);
  inst::instance* string_ctor_string(inst::instance& instance,
                                     const params_t& params);
  inst::instance* string_empty(inst::instance& instance,
                               const params_t& params);
  inst::instance* string_size(inst::instance& instance,
                              const params_t& params);
  inst::instance* string_operator_equal_equal(inst::instance& instance,
                                              const params_t& params);
  inst::instance* string_operator_not_equal(inst::instance& instance,
                                            const params_t& params);
  inst::instance* string_operator_less(inst::instance& instance,
                                       const params_t& params);
  inst::instance* string_operator_less_equal(inst::instance& instance,
                                             const params_t& params);
  inst::instance* string_operator_greater(inst::instance& instance,
                                          const params_t& params);
  inst::instance* string_operator_greater_equal(inst::instance& instance,
                                                const params_t& params);
  inst::instance* string_operator_plus(inst::instance& instance,
                                       const params_t& params);
  inst::instance* string_operator_plus_equal(inst::instance& instance,
                                             const params_t& params);
  inst::instance* string_ctor_string_count(inst::instance& instance,
                                           const params_t& params);
  inst::instance* string_clear(inst::instance& instance,
                               const params_t& params);
  inst::instance* string_insert_pos_str(inst::instance& instance,
                                        const params_t& params);
  inst::instance* string_erase_pos(inst::instance& instance,
                                   const params_t& params);
  inst::instance* string_erase_pos_count(inst::instance& instance,
                                         const params_t& params);
  inst::instance* string_starts_with(inst::instance& instance,
                                     const params_t& params);
  inst::instance* string_ends_with(inst::instance& instance,
                                   const params_t& params);
  inst::instance* string_replace_pos_count_str(inst::instance& instance,
                                               const params_t& params);
  inst::instance* string_substr_pos(inst::instance& instance,
                                    const params_t& params);
  inst::instance* string_substr_pos_count(inst::instance& instance,
                                          const params_t& params);
  inst::instance* string_resize_newsize(inst::instance& instance,
                                        const params_t& params);
  inst::instance* string_resize_newsize_fill(inst::instance& instance,
                                             const params_t& params);
  inst::instance* string_find_str(inst::instance& instance,
                                  const params_t& params);
  inst::instance* string_find_str_pos(inst::instance& instance,
                                      const params_t& params);
  inst::instance* string_find_not_of_str(inst::instance& instance,
                                         const params_t& params);
  inst::instance* string_find_not_of_str_pos(inst::instance& instance,
                                             const params_t& params);
  inst::instance* string_find_last_str(inst::instance& instance,
                                       const params_t& params);
  inst::instance* string_find_last_not_of_str(inst::instance& instance,
                                              const params_t& params);
  inst::instance* string_contains(inst::instance& instance,
                                  const params_t& params);
  inst::instance* string_lower(inst::instance& instance,
                               const params_t& params);
  inst::instance* string_make_lower(inst::instance& instance,
                                    const params_t& params);
  inst::instance* string_upper(inst::instance& instance,
                               const params_t& params);
  inst::instance* string_make_upper(inst::instance& instance,
                                    const params_t& params);

  inst::instance* version_ctor_major(inst::instance& instance,
                                     const params_t& params);
  inst::instance* version_ctor_major_minor(inst::instance& instance,
                                           const params_t& params);
  inst::instance* version_ctor_major_minor_patch(inst::instance& instance,
                                                 const params_t& params);
  inst::instance* version_ctor_major_minor_patch_tweak(
    inst::instance& instance, const params_t& params);
  inst::instance* version_operator_equal_equal(inst::instance& instance,
                                               const params_t& params);
  inst::instance* version_operator_not_equal(inst::instance& instance,
                                             const params_t& params);
  inst::instance* version_operator_less(inst::instance& instance,
                                        const params_t& params);
  inst::instance* version_operator_less_equal(inst::instance& instance,
                                              const params_t& params);
  inst::instance* version_operator_greater(inst::instance& instance,
                                           const params_t& params);
  inst::instance* version_operator_greater_equal(inst::instance& instance,
                                                 const params_t& params);
  inst::instance* version_major(inst::instance& instance,
                                const params_t& params);
  inst::instance* version_minor(inst::instance& instance,
                                const params_t& params);
  inst::instance* version_patch(inst::instance& instance,
                                const params_t& params);
  inst::instance* version_tweak(inst::instance& instance,
                                const params_t& params);
  inst::instance* version_to_string(inst::instance& instance,
                                    const params_t& params);

  inst::instance* list_ctor(inst::instance& instance, const params_t& params);
  inst::instance* list_ctor_list(inst::instance& instance,
                                 const params_t& params);
  inst::instance* list_ctor_value(inst::instance& instance,
                                  const params_t& params);
  inst::instance* list_ctor_value_count(inst::instance& instance,
                                        const params_t& params);
  inst::instance* list_push_back_value(inst::instance& instance,
                                       const params_t& params);
  inst::instance* list_push_back_list(inst::instance& instance,
                                      const params_t& params);
  inst::instance* list_push_front_value(inst::instance& instance,
                                        const params_t& params);
  inst::instance* list_push_front_list(inst::instance& instance,
                                       const params_t& params);
  inst::instance* list_pop_back(inst::instance& instance,
                                const params_t& params);
  inst::instance* list_pop_front(inst::instance& instance,
                                 const params_t& params);
  inst::instance* list_at(inst::instance& instance, const params_t& params);
  inst::instance* list_front(inst::instance& instance, const params_t& params);
  inst::instance* list_back(inst::instance& instance, const params_t& params);
  inst::instance* list_insert_pos_value(inst::instance& instance,
                                        const params_t& params);
  inst::instance* list_insert_pos_list(inst::instance& instance,
                                       const params_t& params);
  inst::instance* list_erase_pos(inst::instance& instance,
                                 const params_t& params);
  inst::instance* list_erase_pos_count(inst::instance& instance,
                                       const params_t& params);
  inst::instance* list_remove_value(inst::instance& instance,
                                    const params_t& params);
  inst::instance* list_remove_value_count(inst::instance& instance,
                                          const params_t& params);
  inst::instance* list_remove_last_value_count(inst::instance& instance,
                                               const params_t& params);
  inst::instance* list_clear(inst::instance& instance, const params_t& params);
  inst::instance* list_resize(inst::instance& instance,
                              const params_t& params);
  inst::instance* list_sort(inst::instance& instance, const params_t& params);
  inst::instance* list_reverse(inst::instance& instance,
                               const params_t& params);
  inst::instance* list_min(inst::instance& instance, const params_t& params);
  inst::instance* list_max(inst::instance& instance, const params_t& params);
  inst::instance* list_sublist_pos(inst::instance& instance,
                                   const params_t& params);
  inst::instance* list_sublist_pos_count(inst::instance& instance,
                                         const params_t& params);
  inst::instance* list_size(inst::instance& instance, const params_t& params);
  inst::instance* list_empty(inst::instance& instance, const params_t& params);
  inst::instance* list_find_value(inst::instance& instance,
                                  const params_t& params);
  inst::instance* list_find_value_pos(inst::instance& instance,
                                      const params_t& params);
  inst::instance* list_operator_plus_value(inst::instance& instance,
                                           const params_t& params);
  inst::instance* list_operator_plus_list(inst::instance& instance,
                                          const params_t& params);
  inst::instance* list_operator_plus_equal_value(inst::instance& instance,
                                                 const params_t& params);
  inst::instance* list_operator_plus_equal_list(inst::instance& instance,
                                                const params_t& params);

  inst::instance* project_ctor_name(inst::instance& instance,
                                    const params_t& params);
  inst::instance* project_name(inst::instance& instance,
                               const params_t& params);
  inst::instance* project_add_executable(inst::instance& instance,
                                         const params_t& params);
  inst::instance* project_add_library(inst::instance& instance,
                                      const params_t& params);

  inst::instance* library_name(inst::instance& instance,
                               const params_t& params);
  inst::instance* library_link_to(inst::instance& instance,
                                  const params_t& params);

  inst::instance* executable_name(inst::instance& instance,
                                  const params_t& params);
  inst::instance* executable_link_to(inst::instance& instance,
                                     const params_t& params);

  inst::instance* enum_to_string(inst::instance& instance,
                                 const params_t& params);
  inst::instance* enum_operator_equal(inst::instance& instance,
                                      const params_t& params);
  inst::instance* enum_operator_equalequal(inst::instance& instance,
                                           const params_t& params);
  inst::instance* enum_operator_exclaimequal(inst::instance& instance,
                                             const params_t& params);

  inst::instance* user_type_operator_equal(inst::instance& instance,
                                           const params_t& params);

  inst::instance* option_ctor_description(inst::instance& instance,
                                          const params_t& params);
  inst::instance* option_ctor_description_value(inst::instance& instance,
                                                const params_t& params);
  inst::instance* option_value(inst::instance& instance,
                               const params_t& params);

  inst::instance* cmake_minimum_required(const params_t& params);
  inst::instance* cmake_message(const params_t& params);
  inst::instance* cmake_warning(const params_t& params);
  inst::instance* cmake_error(const params_t& params);
  inst::instance* cmake_fatal_error(const params_t& params);
  inst::instance* cmake_get_cxx_compiler_info(const params_t& params);

private:
  int_t string_pos_to_int(std::string::size_type pos) const;

private:
  static const std::locale m_utf8_locale;

  facade::cmake_facade& m_cmake_facade;
  inst::instances_holder_interface& m_instances;
  const sema::builtin_types_accessor& m_builtin_types;
};
}
}
