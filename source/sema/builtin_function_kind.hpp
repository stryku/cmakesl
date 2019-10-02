#pragma once

namespace cmsl::sema {
enum class builtin_function_kind
{
  // Stand-alone functions:
  cmake_minimum_required,

  // cmake namespace functions:
  cmake_message,
  cmake_warning,
  cmake_error,
  cmake_fatal_error,
  cmake_get_cxx_compiler_info,
  cmake_install_executable,
  cmake_install_library,
  cmake_enable_ctest,
  cmake_add_test,
  cmake_root_source_dir,
  cmake_current_binary_dir,
  cmake_add_custom_command,
  cmake_make_directory,

  // Class methods:

  bool_ctor,
  bool_ctor_bool,
  bool_ctor_int,
  bool_operator_equal,
  bool_operator_equal_equal,
  bool_operator_pipe_pipe,
  bool_operator_amp_amp,
  bool_operator_unary_exclaim,
  bool_to_string,

  int_ctor,
  int_ctor_bool,
  int_ctor_int,
  int_ctor_double,
  int_to_string,
  int_operator_plus,
  int_operator_unary_plusplus,
  int_operator_minus,
  int_operator_unary_minus,
  int_operator_unary_minusminus,
  int_operator_star,
  int_operator_slash,
  int_operator_equal,
  int_operator_plus_equal,
  int_operator_minus_equal,
  int_operator_star_equal,
  int_operator_slash_equal,
  int_operator_less,
  int_operator_less_equal,
  int_operator_greater,
  int_operator_greater_equal,
  int_operator_equal_equal,

  double_ctor,
  double_ctor_double,
  double_ctor_int,
  double_operator_plus,
  double_operator_unary_plusplus,
  double_operator_minus,
  double_operator_unary_minus,
  double_operator_unary_minusminus,
  double_operator_star,
  double_operator_slash,
  double_operator_equal,
  double_operator_plus_equal,
  double_operator_minus_equal,
  double_operator_star_equal,
  double_operator_slash_equal,
  double_operator_less,
  double_operator_less_equal,
  double_operator_greater,
  double_operator_greater_equal,
  double_to_string,

  string_ctor,
  string_ctor_string,
  string_ctor_string_count,
  string_empty,
  string_size,
  string_operator_equal_equal,
  string_operator_not_equal,
  string_operator_less,
  string_operator_less_equal,
  string_operator_greater,
  string_operator_greater_equal,
  string_operator_plus,
  string_operator_plus_equal,
  string_clear,
  string_insert_pos_str,
  string_erase_pos,
  string_erase_pos_count,
  string_starts_with,
  string_ends_with,
  string_replace_pos_count_str,
  string_substr_pos,
  string_substr_pos_count,
  string_resize_newsize,
  string_resize_newsize_fill,
  string_find_str,
  string_find_str_pos,
  string_find_not_of_str,
  string_find_not_of_str_pos,
  string_find_last_str,
  string_find_last_not_of_str,
  string_contains,
  string_lower,
  string_make_lower,
  string_upper,
  string_make_upper,

  version_ctor_major,
  version_ctor_major_minor,
  version_ctor_major_minor_patch,
  version_ctor_major_minor_patch_tweak,
  version_operator_equal_equal,
  version_operator_not_equal,
  version_operator_less,
  version_operator_less_equal,
  version_operator_greater,
  version_operator_greater_equal,
  version_major,
  version_minor,
  version_patch,
  version_tweak,
  version_to_string,

  extern_constructor_name,
  extern_has_value,
  extern_value,

  list_ctor,
  list_ctor_list,
  list_ctor_value,
  list_ctor_value_count,
  list_push_back_value,
  list_push_back_list,
  list_push_front_value,
  list_push_front_list,
  list_pop_back,
  list_pop_front,
  list_at,
  list_front,
  list_back,
  list_insert_pos_value,
  list_insert_pos_list,
  list_erase_pos,
  list_erase_pos_count,
  list_remove_value,
  list_remove_value_count,
  // Todo: Add remove_value_pos* ?
  list_remove_last_value_count,
  list_clear,
  list_resize,
  // Todo: Add resize_fill?
  list_sort,
  list_reverse,
  list_min,
  list_max,
  list_sublist_pos,
  list_sublist_pos_count,
  list_size,
  list_empty,
  list_find_value,
  list_find_value_pos,
  list_operator_plus_value,
  list_operator_plus_list,
  list_operator_plus_equal_value,
  list_operator_plus_equal_list,

  project_ctor_name,
  project_name,
  project_add_executable,
  project_add_library,
  project_find_library,

  library_name,
  library_link_to,
  library_link_to_visibility,
  library_include_directories,
  library_include_directories_visibility,

  executable_name,
  executable_link_to,
  executable_link_to_visibility,
  executable_include_directories,
  executable_include_directories_visibility,

  enum_to_string,
  enum_operator_equalequal,
  enum_operator_exclaimequal,
  enum_operator_equal,

  user_type_operator_equal,

  option_ctor_description,
  option_ctor_description_value,
  option_value
};
}
