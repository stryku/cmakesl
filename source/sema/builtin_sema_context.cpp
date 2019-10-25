#include "sema/builtin_sema_context.hpp"
#include "common/assert.hpp"
#include "qualified_contextes_refs.hpp"
#include "sema/builtin_cmake_namespace_context.hpp"
#include "sema/builtin_function_kind.hpp"
#include "sema/builtin_sema_function.hpp"
#include "sema/builtin_token_provider.hpp"
#include "sema/factories.hpp"
#include "sema/factories_provider.hpp"
#include "sema/generic_type_creation_utils.hpp"
#include "sema/type_builder.hpp"

#include <generated/builtin_token_providers.hpp>

namespace cmsl::sema {
builtin_sema_context::builtin_sema_context(
  factories_provider& factories, errors::errors_observer& errors_observer,
  const builtin_token_provider& builtin_token_provider,
  qualified_contextes_refs& qualified_ctxs)
  : builtin_context_base{ qualified_ctxs, factories, builtin_token_provider,
                          "" }
  , m_errors_observer{ errors_observer }
{
  add_types();
  add_functions();
  add_cmake_namespace_context();
}

builtin_sema_context::~builtin_sema_context()
{
}

void builtin_sema_context::add_types()
{
  auto bool_manipulator = add_bool_type();
  auto int_manipulator = add_int_type();
  auto double_manipulator = add_double_type();
  auto string_manipulator = add_string_type();
  auto void_manipulator = add_void_type();

  const auto& [void_, _] = void_manipulator.built_type();
  (void)_;
  const auto& [bool_, bool_ref] = bool_manipulator.built_type();
  const auto& [int_, int_ref] = int_manipulator.built_type();
  const auto& [double_, double_ref] = double_manipulator.built_type();
  const auto& [string, string_ref] = string_manipulator.built_type();

  const auto types = builtin_types_accessor{ .void_ = void_,
                                             .bool_ = bool_,
                                             .bool_ref = bool_ref,
                                             .int_ = int_,
                                             .int_ref = int_ref,
                                             .double_ = double_,
                                             .double_ref = double_ref,
                                             .string = string,
                                             .string_ref = string_ref };

  m_builtin_types = std::make_unique<builtin_types_accessor>(types);

  m_generics_creation_utils = std::make_unique<generic_type_creation_utils>(
    *this, m_factories, m_errors_observer, m_builtin_tokens, *m_builtin_types,
    m_qualified_ctxs.types);

  add_bool_member_functions(bool_manipulator);
  add_int_member_functions(int_manipulator);
  add_double_member_functions(double_manipulator);
  add_string_member_functions(string_manipulator);
}

void builtin_sema_context::add_functions()
{
  // No functions yet.
}

type_builder builtin_sema_context::add_bool_type()
{
  static const auto token = m_builtin_tokens.bool_().name();
  return add_type_and_get_builder(token);
}

void builtin_sema_context::add_bool_member_functions(
  type_builder& bool_manipulator)
{
  const auto& int_type = m_builtin_types->int_;
  const auto& bool_type = m_builtin_types->bool_;
  const auto& string_type = m_builtin_types->string;
  const auto& bool_reference_type = m_builtin_types->bool_ref;

  const auto token_provider = m_builtin_tokens.bool_();

  const builtin_functions_t functions = {
    builtin_function_info{ // bool()
                           builtin_function_kind::bool_ctor, bool_type,
                           token_provider.default_constructor() },
    builtin_function_info{ // bool(bool)
                           builtin_function_kind::bool_ctor_bool,
                           bool_type,
                           token_provider.copy_constructor(),
                           { bool_type } },
    builtin_function_info{ // bool(int)
                           builtin_function_kind::bool_ctor_int,
                           bool_type,
                           token_provider.conversion_from_int_constructor(),
                           { int_type } },
    builtin_function_info{ // operator=(bool)
                           builtin_function_kind::bool_operator_equal,
                           bool_reference_type,
                           token_provider.operator_equal(),
                           { bool_type } },
    builtin_function_info{ // operator==(bool)
                           builtin_function_kind::bool_operator_equal_equal,
                           bool_type,
                           token_provider.operator_equal_equal(),
                           { bool_type } },
    builtin_function_info{ // operator||(bool)
                           builtin_function_kind::bool_operator_pipe_pipe,
                           bool_type,
                           token_provider.operator_pipe_pipe(),
                           { bool_type } },
    builtin_function_info{ // operator&&(bool)
                           builtin_function_kind::bool_operator_amp_amp,
                           bool_type,
                           token_provider.operator_amp_amp(),
                           { bool_type } },
    builtin_function_info{ // operator!()
                           builtin_function_kind::bool_operator_unary_exclaim,
                           bool_type,
                           token_provider.operator_unary_exclaim() },
    builtin_function_info{ // to_string()
                           builtin_function_kind::bool_to_string, string_type,
                           token_provider.to_string() }
  };

  add_type_member_functions(bool_manipulator, functions);
}

type_builder builtin_sema_context::add_int_type()
{
  static const auto token = m_builtin_tokens.int_().name();
  return add_type_and_get_builder(token);
}

void builtin_sema_context::add_int_member_functions(
  type_builder& int_manipulator)
{
  const auto& int_type = m_builtin_types->int_;
  const auto& int_reference_type = m_builtin_types->int_ref;
  const auto& bool_type = m_builtin_types->bool_;
  const auto& double_type = m_builtin_types->double_;
  const auto& string_type = m_builtin_types->string;

  const auto token_provider = m_builtin_tokens.int_();

  const builtin_functions_t functions = {
    builtin_function_info{ // int()
                           builtin_function_kind::int_ctor, int_type,
                           token_provider.default_constructor() },
    builtin_function_info{ // int(bool)
                           builtin_function_kind::int_ctor_bool,
                           int_type,
                           token_provider.conversion_from_bool_constructor(),
                           { bool_type } },
    builtin_function_info{ // int(int)
                           builtin_function_kind::int_ctor_int,
                           int_type,
                           token_provider.copy_constructor(),
                           { int_type } },
    builtin_function_info{ // int(double)
                           builtin_function_kind::int_ctor_double,
                           int_type,
                           token_provider.conversion_from_double_constructor(),
                           { double_type } },
    builtin_function_info{ // to_string()
                           builtin_function_kind::int_to_string, string_type,
                           token_provider.to_string() },
    builtin_function_info{ // operator+(int)
                           builtin_function_kind::int_operator_plus,
                           int_type,
                           token_provider.operator_plus(),
                           { int_type } },
    builtin_function_info{ // operator++()
                           builtin_function_kind::int_operator_unary_plusplus,
                           int_reference_type,
                           token_provider.operator_unary_plusplus() },
    builtin_function_info{ // operator-(int)
                           builtin_function_kind::int_operator_minus,
                           int_type,
                           token_provider.operator_minus(),
                           { int_type } },
    builtin_function_info{ // operator-()
                           builtin_function_kind::int_operator_unary_minus,
                           int_type, token_provider.operator_unary_minus() },
    builtin_function_info{
      // operator--()
      builtin_function_kind::int_operator_unary_minusminus, int_reference_type,
      token_provider.operator_unary_minusminus() },
    builtin_function_info{ // operator*(int)
                           builtin_function_kind::int_operator_star,
                           int_type,
                           token_provider.operator_star(),
                           { int_type } },
    builtin_function_info{ // operator/(int)
                           builtin_function_kind::int_operator_slash,
                           int_type,
                           token_provider.operator_slash(),
                           { int_type } },
    builtin_function_info{ // operator=(int)
                           builtin_function_kind::int_operator_equal,
                           int_type,
                           token_provider.operator_equal(),
                           { int_type } },
    builtin_function_info{ // int& operator+=(int)
                           builtin_function_kind::int_operator_plus_equal,
                           int_reference_type,
                           token_provider.operator_plus_equal(),
                           { int_type } },
    builtin_function_info{ // int& operator-=(int)
                           builtin_function_kind::int_operator_minus_equal,
                           int_reference_type,
                           token_provider.operator_minus_equal(),
                           { int_type } },
    builtin_function_info{ // int& operator*=(int)
                           builtin_function_kind::int_operator_star_equal,
                           int_reference_type,
                           token_provider.operator_star_equal(),
                           { int_type } },
    builtin_function_info{ // int& operator/=(int)
                           builtin_function_kind::int_operator_slash_equal,
                           int_reference_type,
                           token_provider.operator_slash_equal(),
                           { int_type } },
    builtin_function_info{ // operator<(int)
                           builtin_function_kind::int_operator_less,
                           bool_type,
                           token_provider.operator_less(),
                           { int_type } },
    builtin_function_info{ // operator<=(int)
                           builtin_function_kind::int_operator_less_equal,
                           bool_type,
                           token_provider.operator_less_equal(),
                           { int_type } },
    builtin_function_info{ // operator>(int)
                           builtin_function_kind::int_operator_greater,
                           bool_type,
                           token_provider.operator_greater(),
                           { int_type } },
    builtin_function_info{ // operator>=(int)
                           builtin_function_kind::int_operator_greater_equal,
                           bool_type,
                           token_provider.operator_greater_equal(),
                           { int_type } },
    builtin_function_info{ // operator==(int)
                           builtin_function_kind::int_operator_equal_equal,
                           bool_type,
                           token_provider.operator_equal_equal(),
                           { int_type } }
  };

  add_type_member_functions(int_manipulator, functions);
}

type_builder builtin_sema_context::add_double_type()
{
  static const auto token = m_builtin_tokens.double_().name();
  return add_type_and_get_builder(token);
}

void builtin_sema_context::add_double_member_functions(
  type_builder& double_manipulator)
{
  const auto& int_type = m_builtin_types->int_;
  const auto& bool_type = m_builtin_types->bool_;
  const auto& double_type = m_builtin_types->double_;
  const auto& double_reference_type = m_builtin_types->double_ref;
  const auto& string_type = m_builtin_types->string;

  const auto token_provider = m_builtin_tokens.double_();

  const builtin_functions_t functions = {
    builtin_function_info{ // double()
                           builtin_function_kind::double_ctor, double_type,
                           token_provider.default_constructor() },
    builtin_function_info{ // double(double)
                           builtin_function_kind::double_ctor_double,
                           double_type,
                           token_provider.copy_constructor(),
                           { double_type } },
    builtin_function_info{ // double(int)
                           builtin_function_kind::double_ctor_int,
                           double_type,
                           token_provider.conversion_from_int_constructor(),
                           { int_type } },
    builtin_function_info{ // operator+(double)
                           builtin_function_kind::double_operator_plus,
                           double_type,
                           token_provider.operator_plus(),
                           { double_type } },
    builtin_function_info{
      // operator++()
      builtin_function_kind::double_operator_unary_plusplus,
      double_reference_type,
      token_provider.operator_unary_plusplus(),
    },
    builtin_function_info{ // operator-(double)
                           builtin_function_kind::double_operator_minus,
                           double_type,
                           token_provider.operator_minus(),
                           { double_type } },
    builtin_function_info{ // operator-()
                           builtin_function_kind::double_operator_unary_minus,
                           double_type,
                           token_provider.operator_unary_minus() },
    builtin_function_info{
      // operator--()
      builtin_function_kind::double_operator_unary_minusminus,
      double_reference_type, token_provider.operator_unary_minusminus() },
    builtin_function_info{ // operator*(double)
                           builtin_function_kind::double_operator_star,
                           double_type,
                           token_provider.operator_start(),
                           { double_type } },
    builtin_function_info{ // operator/(double)
                           builtin_function_kind::double_operator_slash,
                           double_type,
                           token_provider.operator_slash(),
                           { double_type } },
    builtin_function_info{ // double& operator=(double)
                           builtin_function_kind::double_operator_equal,
                           double_reference_type,
                           token_provider.operator_equal(),
                           { double_type } },
    builtin_function_info{ // double&operator+=(double)
                           builtin_function_kind::double_operator_plus_equal,
                           double_reference_type,
                           token_provider.operator_plus_equal(),
                           { double_type } },
    builtin_function_info{ // double&operator-=(double)
                           builtin_function_kind::double_operator_minus_equal,
                           double_reference_type,
                           token_provider.operator_minus_equal(),
                           { double_type } },
    builtin_function_info{ // double& operator*=(double)
                           builtin_function_kind::double_operator_star_equal,
                           double_reference_type,
                           token_provider.operator_start_equal(),
                           { double_type } },
    builtin_function_info{ // double& operator/=(double)
                           builtin_function_kind::double_operator_slash_equal,
                           double_reference_type,
                           token_provider.operator_slash_equal(),
                           { double_type } },
    builtin_function_info{ // operator<(double)
                           builtin_function_kind::double_operator_less,
                           bool_type,
                           token_provider.operator_less(),
                           { double_type } },
    builtin_function_info{ // operator<=(double)
                           builtin_function_kind::double_operator_less_equal,
                           bool_type,
                           token_provider.operator_less_equal(),
                           { double_type } },
    builtin_function_info{ // operator>(double)
                           builtin_function_kind::double_operator_greater,
                           bool_type,
                           token_provider.operator_greater(),
                           { double_type } },
    builtin_function_info{
      // operator>=(double)
      builtin_function_kind::double_operator_greater_equal,
      bool_type,
      token_provider.operator_greater_equal(),
      { double_type } },
    builtin_function_info{ builtin_function_kind::double_to_string,
                           string_type, token_provider.to_string() }
  };

  add_type_member_functions(double_manipulator, functions);
}

type_builder builtin_sema_context::add_string_type()
{
  static const auto token = m_builtin_tokens.string().name();
  return add_type_and_get_builder(token);
}

void builtin_sema_context::add_string_member_functions(
  type_builder& string_manipulator)
{
  const auto& int_type = m_builtin_types->int_;
  const auto& bool_type = m_builtin_types->bool_;
  const auto& string_type = m_builtin_types->string;
  const auto& string_reference_type = m_builtin_types->string_ref;
  const auto& void_type = m_builtin_types->string;

  const auto token_provider = m_builtin_tokens.string();

  const builtin_functions_t functions = {
    builtin_function_info{ // string()
                           builtin_function_kind::string_ctor, string_type,
                           token_provider.default_constructor() },
    builtin_function_info{ // string(string)
                           builtin_function_kind::string_ctor_string,
                           string_type,
                           token_provider.copy_constructor(),
                           { string_type } },
    builtin_function_info{ // string(string str, int count)
                           builtin_function_kind::string_ctor_string_count,
                           string_type,
                           token_provider.value_initialization_constructor(),
                           { string_type, int_type } },
    builtin_function_info{ // bool empty()
                           builtin_function_kind::string_empty, bool_type,
                           token_provider.empty() },
    builtin_function_info{ // int size()
                           builtin_function_kind::string_size, int_type,
                           token_provider.size() },
    builtin_function_info{ // bool operator==(string)
                           builtin_function_kind::string_operator_equal_equal,
                           bool_type,
                           token_provider.operator_equal_equal(),
                           { string_type } },
    builtin_function_info{ // bool operator!=(string)
                           builtin_function_kind::string_operator_not_equal,
                           bool_type,
                           token_provider.operator_not_equal(),
                           { string_type } },
    builtin_function_info{ // bool operator<(string)
                           builtin_function_kind::string_operator_less,
                           bool_type,
                           token_provider.operator_less(),
                           { string_type } },
    builtin_function_info{ // bool operator<=(string)
                           builtin_function_kind::string_operator_less_equal,
                           bool_type,
                           token_provider.operator_less_equal(),
                           { string_type } },
    builtin_function_info{ // bool operator>(string)
                           builtin_function_kind::string_operator_greater,
                           bool_type,
                           token_provider.operator_greater(),
                           { string_type } },
    builtin_function_info{
      // bool operator>=(string)
      builtin_function_kind::string_operator_greater_equal,
      bool_type,
      token_provider.operator_greater_equal(),
      { string_type } },
    builtin_function_info{ // string operator+(string)
                           builtin_function_kind::string_operator_plus,
                           string_type,
                           token_provider.operator_plus(),
                           { string_type } },
    builtin_function_info{ // string& operator+=(string)
                           builtin_function_kind::string_operator_plus_equal,
                           string_reference_type,
                           token_provider.operator_plus_equal(),
                           { string_type } },

    builtin_function_info{ // string operator/(string)
                           builtin_function_kind::string_operator_slash,
                           string_type,
                           token_provider.operator_slash(),
                           { string_type } },
    builtin_function_info{ // string& operator/=(string)
                           builtin_function_kind::string_operator_slash_equal,
                           string_reference_type,
                           token_provider.operator_slash_equal(),
                           { string_type } },

    // Todo: clear could return a reference
    builtin_function_info{ // void clear()
                           builtin_function_kind::string_clear, void_type,
                           token_provider.clear() },
    builtin_function_info{
      // string& insert(int position, string str)
      builtin_function_kind::string_insert_pos_str,
      string_reference_type,
      token_provider.insert(),
      { int_type, string_type },

    },
    builtin_function_info{ // string& erase(int position)
                           builtin_function_kind::string_erase_pos,
                           string_reference_type,
                           token_provider.erase_pos(),
                           { int_type } },
    builtin_function_info{
      // string& erase(int position, int count)
      builtin_function_kind::string_erase_pos_count,
      string_reference_type,
      token_provider.erase_pos_count(),
      { int_type, int_type },

    },
    builtin_function_info{ // bool starts_with(string str)
                           builtin_function_kind::string_starts_with,
                           bool_type,
                           token_provider.starts_with(),
                           { string_type } },
    builtin_function_info{ // bool ends_with(string str)
                           builtin_function_kind::string_ends_with,
                           bool_type,
                           token_provider.ends_with(),
                           { string_type } },
    builtin_function_info{
      // string& replace(int pos, int count, string str)
      builtin_function_kind::string_replace_pos_count_str,
      string_reference_type,
      token_provider.replace(),
      { int_type, int_type, string_type },
    },
    builtin_function_info{ // string substr(int pos)
                           builtin_function_kind::string_substr_pos,
                           string_type,
                           token_provider.substr_pos(),
                           { int_type } },
    builtin_function_info{
      // string substr(int pos, int count)
      builtin_function_kind::string_substr_pos_count,
      string_type,
      token_provider.substr_pos_count(),
      { int_type, int_type },
    },
    // Todo: resize could return a reference
    builtin_function_info{ // void resize(int new_size)
                           builtin_function_kind::string_resize_newsize,
                           void_type,
                           token_provider.resize_size(),
                           { int_type } },
    // Todo: resize could return a reference
    builtin_function_info{
      // void resize(int new_size, string fill)
      builtin_function_kind::string_resize_newsize_fill,
      void_type,
      token_provider.resize_size_fill(),
      { int_type, string_type },
    },
    builtin_function_info{ // int find(string str)
                           builtin_function_kind::string_find_str,
                           int_type,
                           token_provider.find_str(),
                           { string_type } },
    builtin_function_info{
      // int find(string str, int pos)
      builtin_function_kind::string_find_str_pos,
      int_type,
      token_provider.find_str_pos(),
      { string_type, int_type },
    },
    builtin_function_info{ // int find_not_of(string str)
                           builtin_function_kind::string_find_not_of_str,
                           int_type,
                           token_provider.find_not_of_str(),
                           { string_type } },
    builtin_function_info{
      // int find_not_of(string str, int pos)
      builtin_function_kind::string_find_not_of_str_pos,
      int_type,
      token_provider.find_not_of_str_pos(),
      { string_type, int_type },

    },
    builtin_function_info{ // int find_last(string str)
                           builtin_function_kind::string_find_last_str,
                           int_type,
                           token_provider.find_last(),
                           { string_type } },
    builtin_function_info{ // int find_last_not_of(string str)
                           builtin_function_kind::string_find_last_not_of_str,
                           int_type,
                           token_provider.find_last_not_of(),
                           { string_type } },
    builtin_function_info{ // bool contains(string str)
                           builtin_function_kind::string_contains,
                           bool_type,
                           token_provider.contains(),
                           { string_type } },
    // Todo: lower should return reference
    builtin_function_info{ // void lower()
                           builtin_function_kind::string_lower, void_type,
                           token_provider.lower() },
    builtin_function_info{ // string make_lower()
                           builtin_function_kind::string_make_lower,
                           string_type, token_provider.make_lower() },
    // Todo: upper should return reference
    builtin_function_info{ // void upper()
                           builtin_function_kind::string_upper, void_type,
                           token_provider.upper() },
    builtin_function_info{ // string make_upper()
                           builtin_function_kind::string_make_upper,
                           string_type, token_provider.make_upper() }
  };

  add_type_member_functions(string_manipulator, functions);
}

type_builder builtin_sema_context::add_void_type()
{
  static const auto token = m_builtin_tokens.void_().name();
  return add_type_and_get_builder(token);
}

builtin_types_accessor builtin_sema_context::builtin_types() const
{
  return *m_builtin_types;
}

void builtin_sema_context::add_cmake_namespace_context()
{
  m_cmake_namespace_context =
    std::make_unique<builtin_cmake_namespace_context>(
      *this, m_qualified_ctxs, m_factories, m_builtin_tokens, *m_builtin_types,
      *m_generics_creation_utils);

  m_builtin_types->cmake = &(m_cmake_namespace_context->types_accessor());
}

std::vector<builtin_identifier_info>
builtin_sema_context::builtin_identifiers_info() const
{
  return m_cmake_namespace_context->builtin_identifiers_info();
}
}
