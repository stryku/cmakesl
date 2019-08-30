#include "sema/builtin_sema_context.hpp"
#include "common/assert.hpp"
#include "qualified_contextes.hpp"
#include "sema/builtin_cmake_namespace_context.hpp"
#include "sema/builtin_function_kind.hpp"
#include "sema/builtin_sema_function.hpp"
#include "sema/builtin_token_provider.hpp"
#include "sema/builtin_types_finder.hpp"
#include "sema/factories.hpp"
#include "sema/type_builder.hpp"

#include <generated/builtin_token_providers.hpp>

namespace cmsl::sema {
builtin_sema_context::builtin_sema_context(
  sema_type_factory& type_factory, sema_function_factory& function_factory,
  sema_context_factory& context_factory,
  errors::errors_observer& errors_observer,
  const builtin_token_provider& builtin_token_provider,
  qualified_contextes& qualified_ctxs)
  : sema_context_impl{ "" }
  , m_type_factory{ type_factory }
  , m_function_factory{ function_factory }
  , m_context_factory{ context_factory }
  , m_errors_observer{ errors_observer }
  , m_builtin_token_provider{ builtin_token_provider }
  , m_qualified_ctxs{ qualified_ctxs }
{
  add_types();
  add_functions();
  add_cmake_namespace_context();
}

template <unsigned N>
lexer::token builtin_sema_context::make_token(lexer::token_type token_type,
                                              const char (&tok)[N])
{
  // N counts also '\0'
  const auto src_range = source_range{ source_location{ 1u, 1u, 0u },
                                       source_location{ 1u, N, N - 1u } };
  return lexer::token{ token_type, src_range, cmsl::source_view{ tok } };
}

template <unsigned N>
lexer::token builtin_sema_context::make_id_token(const char (&tok)[N])
{
  return make_token(token_type_t::identifier, tok);
}

void builtin_sema_context::add_types()
{
  auto bool_manipulator = add_bool_type();
  auto int_manipulator = add_int_type();
  auto double_manipulator = add_double_type();
  auto string_manipulator = add_string_type();
  auto version_manipulator = add_version_type();
  auto library_manipulator = add_library_type();
  auto executable_manipulator = add_executable_type();
  auto project_manipulator = add_project_type();
  auto option_manipulator = add_option_type();
  auto void_manipulator = add_void_type();

  const auto& [void_, _] = void_manipulator.built_type();
  const auto& [bool_, bool_ref] = bool_manipulator.built_type();
  const auto& [int_, int_ref] = int_manipulator.built_type();
  const auto& [double_, double_ref] = double_manipulator.built_type();
  const auto& [string, string_ref] = string_manipulator.built_type();
  const auto& [version, version_ref] = version_manipulator.built_type();
  const auto& [library, library_ref] = library_manipulator.built_type();
  const auto& [executable, executable_ref] =
    executable_manipulator.built_type();
  const auto& [project, project_ref] = project_manipulator.built_type();
  const auto& [option, option_ref] = option_manipulator.built_type();

  const auto types = builtin_types_accessor{ .void_ = void_,
                                             .bool_ = bool_,
                                             .bool_ref = bool_ref,
                                             .int_ = int_,
                                             .int_ref = int_ref,
                                             .double_ = double_,
                                             .double_ref = double_ref,
                                             .string = string,
                                             .string_ref = string_ref,
                                             .version = version,
                                             .version_ref = version_ref,
                                             .library = library,
                                             .library_ref = library_ref,
                                             .executable = executable,
                                             .executable_ref = executable_ref,
                                             .project = project,
                                             .project_ref = project_ref,
                                             .option = option,
                                             .option_ref = option_ref };

  m_builtin_types = std::make_unique<builtin_types_accessor>(types);

  add_bool_member_functions(bool_manipulator);
  add_int_member_functions(int_manipulator);
  add_double_member_functions(double_manipulator);
  add_string_member_functions(string_manipulator);
  add_version_member_functions(version_manipulator);
  add_library_member_functions(library_manipulator);
  add_executable_member_functions(executable_manipulator);
  add_project_member_functions(project_manipulator);
  add_option_member_functions(option_manipulator);
}

void builtin_sema_context::add_functions()
{
  const auto& version_type = m_builtin_types->version;
  const auto& void_type = m_builtin_types->void_;

  const auto functions = { builtin_function_info{
    // void cmake_minimum_required(version)
    void_type,
    function_signature{
      make_id_token("cmake_minimum_required"),
      { parameter_declaration{ version_type, make_id_token("") } } },
    builtin_function_kind::cmake_minimum_required } };

  for (const auto& function : functions) {
    const auto& created = m_function_factory.create_builtin(
      *this, function.return_type, std::move(function.signature),
      function.kind);
    add_function(created);
  }
}

template <typename Functions>
void builtin_sema_context::add_type_member_functions(type_builder& manipulator,
                                                     Functions&& functions)
{
  for (const auto& function : functions) {
    manipulator.with_builtin_function(
      function.return_type, std::move(function.signature), function.kind);
  }
}

type_builder builtin_sema_context::add_bool_type()
{
  static const auto token = m_builtin_token_provider.bool_().name();
  return add_type(token);
}

void builtin_sema_context::add_bool_member_functions(
  type_builder& bool_manipulator)
{
  const auto& int_type = m_builtin_types->int_;
  const auto& bool_type = m_builtin_types->bool_;
  const auto& string_type = m_builtin_types->string;
  const auto& bool_reference_type = m_builtin_types->bool_ref;

  const auto token_provider = m_builtin_token_provider.bool_();

  const auto functions = {
    builtin_function_info{
      // bool()
      bool_type, function_signature{ token_provider.default_constructor() },
      builtin_function_kind::bool_ctor },
    builtin_function_info{
      // bool(bool)
      bool_type,
      function_signature{
        token_provider.copy_constructor(),
        { parameter_declaration{ bool_type, make_id_token("") } } },
      builtin_function_kind::bool_ctor_bool },
    builtin_function_info{
      // bool(int)
      bool_type,
      function_signature{
        token_provider.conversion_from_int_constructor(),
        { parameter_declaration{ int_type, make_id_token("") } } },
      builtin_function_kind::bool_ctor_int },
    builtin_function_info{
      // operator=(bool)
      bool_reference_type,
      function_signature{
        token_provider.operator_equal(),
        { parameter_declaration{ bool_type, make_id_token("") } } },
      builtin_function_kind::bool_operator_equal },
    builtin_function_info{
      // operator==(bool)
      bool_type,
      function_signature{
        token_provider.operator_equal_equal(),
        { parameter_declaration{ bool_type, make_id_token("") } } },
      builtin_function_kind::bool_operator_equal_equal },
    builtin_function_info{
      // operator||(bool)
      bool_type,
      function_signature{
        token_provider.operator_pipe_pipe(),
        { parameter_declaration{ bool_type, make_id_token("") } } },
      builtin_function_kind::bool_operator_pipe_pipe },
    builtin_function_info{
      // operator&&(bool)
      bool_type,
      function_signature{
        token_provider.operator_amp_amp(),
        { parameter_declaration{ bool_type, make_id_token("") } } },
      builtin_function_kind::bool_operator_amp_amp },
    builtin_function_info{
      // operator!()
      bool_type,
      function_signature{ token_provider.operator_unary_exclaim(), {} },
      builtin_function_kind::bool_operator_unary_exclaim },
    builtin_function_info{ // to_string()
                           string_type,
                           function_signature{ token_provider.to_string() },
                           builtin_function_kind::bool_to_string }
  };

  add_type_member_functions(bool_manipulator, functions);
}

type_builder builtin_sema_context::add_int_type()
{
  static const auto token = m_builtin_token_provider.int_().name();
  return add_type(token);
}

void builtin_sema_context::add_int_member_functions(
  type_builder& int_manipulator)
{
  const auto& int_type = m_builtin_types->int_;
  const auto& int_reference_type = m_builtin_types->int_ref;
  const auto& bool_type = m_builtin_types->bool_;
  const auto& double_type = m_builtin_types->double_;
  const auto& string_type = m_builtin_types->string;

  const auto token_provider = m_builtin_token_provider.int_();

  const auto functions = {
    builtin_function_info{
      // int()
      int_type, function_signature{ token_provider.default_constructor() },
      builtin_function_kind::int_ctor },
    builtin_function_info{
      // int(bool)
      int_type,
      function_signature{
        token_provider.conversion_from_bool_constructor(),
        { parameter_declaration{ bool_type, make_id_token("") } } },
      builtin_function_kind::int_ctor_bool },
    builtin_function_info{
      // int(int)
      int_type,
      function_signature{
        token_provider.copy_constructor(),
        { parameter_declaration{ int_type, make_id_token("") } } },
      builtin_function_kind::int_ctor_int },
    builtin_function_info{
      // int(double)
      int_type,
      function_signature{
        token_provider.conversion_from_double_constructor(),
        { parameter_declaration{ double_type, make_id_token("") } } },
      builtin_function_kind::int_ctor_double },
    builtin_function_info{
      // to_string()
      string_type, function_signature{ token_provider.to_string(), {} },
      builtin_function_kind::int_to_string },
    builtin_function_info{
      // operator+(int)
      int_type,
      function_signature{
        token_provider.operator_plus(),
        { parameter_declaration{ int_type, make_id_token("") } } },
      builtin_function_kind::int_operator_plus },
    builtin_function_info{
      // operator++()
      int_reference_type,
      function_signature{ token_provider.operator_unary_plusplus(), {} },
      builtin_function_kind::int_operator_unary_plusplus },
    builtin_function_info{
      // operator-(int)
      int_type,
      function_signature{
        token_provider.operator_minus(),
        { parameter_declaration{ int_type, make_id_token("") } } },
      builtin_function_kind::int_operator_minus },
    builtin_function_info{
      // operator-()
      int_type,
      function_signature{ token_provider.operator_unary_minus(), {} },
      builtin_function_kind::int_operator_unary_minus },
    builtin_function_info{
      // operator--()
      int_reference_type,
      function_signature{ token_provider.operator_unary_minusminus(), {} },
      builtin_function_kind::int_operator_unary_minusminus },
    builtin_function_info{
      // operator*(int)
      int_type,
      function_signature{
        token_provider.operator_star(),
        { parameter_declaration{ int_type, make_id_token("") } } },
      builtin_function_kind::int_operator_star },
    builtin_function_info{
      // operator/(int)
      int_type,
      function_signature{
        token_provider.operator_slash(),
        { parameter_declaration{ int_type, make_id_token("") } } },
      builtin_function_kind::int_operator_slash },
    builtin_function_info{
      // operator=(int)
      int_type,
      function_signature{
        token_provider.operator_equal(),
        { parameter_declaration{ int_type, make_id_token("") } } },
      builtin_function_kind::int_operator_equal },
    builtin_function_info{
      // int& operator+=(int)
      int_reference_type,
      function_signature{
        token_provider.operator_plus_equal(),
        { parameter_declaration{ int_type, make_id_token("") } } },
      builtin_function_kind::int_operator_plus_equal },
    builtin_function_info{
      // int& operator-=(int)
      int_reference_type,
      function_signature{
        token_provider.operator_minus_equal(),
        { parameter_declaration{ int_type, make_id_token("") } } },
      builtin_function_kind::int_operator_minus_equal },
    builtin_function_info{
      // int& operator*=(int)
      int_reference_type,
      function_signature{
        token_provider.operator_star_equal(),
        { parameter_declaration{ int_type, make_id_token("") } } },
      builtin_function_kind::int_operator_star_equal },
    builtin_function_info{
      // int& operator/=(int)
      int_reference_type,
      function_signature{
        token_provider.operator_slash_equal(),
        { parameter_declaration{ int_type, make_id_token("") } } },
      builtin_function_kind::int_operator_slash_equal },
    builtin_function_info{
      // operator<(int)
      bool_type,
      function_signature{
        token_provider.operator_less(),
        { parameter_declaration{ int_type, make_id_token("") } } },
      builtin_function_kind::int_operator_less },
    builtin_function_info{
      // operator<=(int)
      bool_type,
      function_signature{
        token_provider.operator_less_equal(),
        { parameter_declaration{ int_type, make_id_token("") } } },
      builtin_function_kind::int_operator_less_equal },
    builtin_function_info{
      // operator>(int)
      bool_type,
      function_signature{
        token_provider.operator_greater(),
        { parameter_declaration{ int_type, make_id_token("") } } },
      builtin_function_kind::int_operator_greater },
    builtin_function_info{
      // operator>=(int)
      bool_type,
      function_signature{
        token_provider.operator_greater_equal(),
        { parameter_declaration{ int_type, make_id_token("") } } },
      builtin_function_kind::int_operator_greater_equal },
    builtin_function_info{
      // operator==(int)
      bool_type,
      function_signature{
        token_provider.operator_equal_equal(),
        { parameter_declaration{ int_type, make_id_token("") } } },
      builtin_function_kind::int_operator_equal_equal }
  };

  add_type_member_functions(int_manipulator, functions);
}

type_builder builtin_sema_context::add_double_type()
{
  static const auto token = m_builtin_token_provider.double_().name();
  return add_type(token);
}

void builtin_sema_context::add_double_member_functions(
  type_builder& double_manipulator)
{
  const auto& int_type = m_builtin_types->int_;
  const auto& bool_type = m_builtin_types->bool_;
  const auto& double_type = m_builtin_types->double_;
  const auto& double_reference_type = m_builtin_types->double_ref;
  const auto& string_type = m_builtin_types->string;

  const auto token_provider = m_builtin_token_provider.double_();

  const auto functions = {
    builtin_function_info{
      // double()
      double_type, function_signature{ token_provider.default_constructor() },
      builtin_function_kind::double_ctor },
    builtin_function_info{
      // double(double)
      double_type,
      function_signature{
        token_provider.copy_constructor(),
        { parameter_declaration{ double_type, make_id_token("") } } },
      builtin_function_kind::double_ctor_double },
    builtin_function_info{
      // double(int)
      double_type,
      function_signature{
        token_provider.conversion_from_int_constructor(),
        { parameter_declaration{ int_type, make_id_token("") } } },
      builtin_function_kind::double_ctor_int },
    builtin_function_info{
      // operator+(double)
      double_type,
      function_signature{
        token_provider.operator_plus(),
        { parameter_declaration{ double_type, make_id_token("") } } },
      builtin_function_kind::double_operator_plus },
    builtin_function_info{
      // operator++()
      double_reference_type,
      function_signature{ token_provider.operator_unary_plusplus(), {} },
      builtin_function_kind::double_operator_unary_plusplus },
    builtin_function_info{
      // operator-(double)
      double_type,
      function_signature{
        token_provider.operator_minus(),
        { parameter_declaration{ double_type, make_id_token("") } } },
      builtin_function_kind::double_operator_minus },
    builtin_function_info{
      // operator-()
      double_type,
      function_signature{ token_provider.operator_unary_minus(), {} },
      builtin_function_kind::double_operator_unary_minus },
    builtin_function_info{
      // operator--()
      double_reference_type,
      function_signature{ token_provider.operator_unary_minusminus(), {} },
      builtin_function_kind::double_operator_unary_minusminus },
    builtin_function_info{
      // operator*(double)
      double_type,
      function_signature{
        token_provider.operator_start(),
        { parameter_declaration{ double_type, make_id_token("") } } },
      builtin_function_kind::double_operator_star },
    builtin_function_info{
      // operator/(double)
      double_type,
      function_signature{
        token_provider.operator_slash(),
        { parameter_declaration{ double_type, make_id_token("") } } },
      builtin_function_kind::double_operator_slash },
    builtin_function_info{
      // double& operator=(double)
      double_reference_type,
      function_signature{
        token_provider.operator_equal(),
        { parameter_declaration{ double_type, make_id_token("") } } },
      builtin_function_kind::double_operator_equal },
    builtin_function_info{
      // double&operator+=(double)
      double_reference_type,
      function_signature{
        token_provider.operator_plus_equal(),
        { parameter_declaration{ double_type, make_id_token("") } } },
      builtin_function_kind::double_operator_plus_equal },
    builtin_function_info{
      // double&operator-=(double)
      double_reference_type,
      function_signature{
        token_provider.operator_minus_equal(),
        { parameter_declaration{ double_type, make_id_token("") } } },
      builtin_function_kind::double_operator_minus_equal },
    builtin_function_info{
      // double& operator*=(double)
      double_reference_type,
      function_signature{
        token_provider.operator_start_equal(),
        { parameter_declaration{ double_type, make_id_token("") } } },
      builtin_function_kind::double_operator_star_equal },
    builtin_function_info{
      // double& operator/=(double)
      double_reference_type,
      function_signature{
        token_provider.operator_slash_equal(),
        { parameter_declaration{ double_type, make_id_token("") } } },
      builtin_function_kind::double_operator_slash_equal },
    builtin_function_info{
      // operator<(double)
      bool_type,
      function_signature{
        token_provider.operator_less(),
        { parameter_declaration{ double_type, make_id_token("") } } },
      builtin_function_kind::double_operator_less },
    builtin_function_info{
      // operator<=(double)
      bool_type,
      function_signature{
        token_provider.operator_less_equal(),
        { parameter_declaration{ double_type, make_id_token("") } } },
      builtin_function_kind::double_operator_less_equal },
    builtin_function_info{
      // operator>(double)
      bool_type,
      function_signature{
        token_provider.operator_greater(),
        { parameter_declaration{ double_type, make_id_token("") } } },
      builtin_function_kind::double_operator_greater },
    builtin_function_info{
      // operator>=(double)
      bool_type,
      function_signature{
        token_provider.operator_greater_equal(),
        { parameter_declaration{ double_type, make_id_token("") } } },
      builtin_function_kind::double_operator_greater_equal },
    builtin_function_info{ string_type,
                           function_signature{ token_provider.to_string() },
                           builtin_function_kind::double_to_string }
  };

  add_type_member_functions(double_manipulator, functions);
}

type_builder builtin_sema_context::add_string_type()
{
  static const auto token = m_builtin_token_provider.string().name();
  return add_type(token);
}

void builtin_sema_context::add_string_member_functions(
  type_builder& string_manipulator)
{
  const auto& int_type = m_builtin_types->int_;
  const auto& bool_type = m_builtin_types->bool_;
  const auto& string_type = m_builtin_types->string;
  const auto& string_reference_type = m_builtin_types->string_ref;
  const auto& void_type = m_builtin_types->string;

  const auto token_provider = m_builtin_token_provider.string();

  const auto functions = {
    builtin_function_info{
      // string()
      string_type, function_signature{ token_provider.default_constructor() },
      builtin_function_kind::string_ctor },
    builtin_function_info{
      // string(string)
      string_type,
      function_signature{
        token_provider.copy_constructor(),
        { parameter_declaration{ string_type, make_id_token("") } } },
      builtin_function_kind::string_ctor_string },
    builtin_function_info{
      // string(string str, int count)
      string_type,
      function_signature{
        token_provider.value_initialization_constructor(),
        { parameter_declaration{ string_type, make_id_token("") },
          parameter_declaration{ int_type, make_id_token("") } } },
      builtin_function_kind::string_ctor_string_count },
    builtin_function_info{ // bool empty()
                           bool_type,
                           function_signature{ token_provider.empty() },
                           builtin_function_kind::string_empty },
    builtin_function_info{ // int size()
                           int_type,
                           function_signature{ token_provider.size() },
                           builtin_function_kind::string_size },
    builtin_function_info{
      // bool operator==(string)
      bool_type,
      function_signature{
        token_provider.operator_equal_equal(),
        { parameter_declaration{ string_type, make_id_token("") } } },
      builtin_function_kind::string_operator_equal_equal },
    builtin_function_info{
      // bool operator!=(string)
      bool_type,
      function_signature{
        token_provider.operator_not_equal(),
        { parameter_declaration{ string_type, make_id_token("") } } },
      builtin_function_kind::string_operator_not_equal },
    builtin_function_info{
      // bool operator<(string)
      bool_type,
      function_signature{
        token_provider.operator_less(),
        { parameter_declaration{ string_type, make_id_token("") } } },
      builtin_function_kind::string_operator_less },
    builtin_function_info{
      // bool operator<=(string)
      bool_type,
      function_signature{
        token_provider.operator_less_equal(),
        { parameter_declaration{ string_type, make_id_token("") } } },
      builtin_function_kind::string_operator_less_equal },
    builtin_function_info{
      // bool operator>(string)
      bool_type,
      function_signature{
        token_provider.operator_greater(),
        { parameter_declaration{ string_type, make_id_token("") } } },
      builtin_function_kind::string_operator_greater },
    builtin_function_info{
      // bool operator>=(string)
      bool_type,
      function_signature{
        token_provider.operator_greater_equal(),
        { parameter_declaration{ string_type, make_id_token("") } } },
      builtin_function_kind::string_operator_greater_equal },
    builtin_function_info{
      // string operator+(string)
      string_type,
      function_signature{
        token_provider.operator_plus(),
        { parameter_declaration{ string_type, make_id_token("") } } },
      builtin_function_kind::string_operator_plus },
    builtin_function_info{
      // string& operator+=(string)
      string_reference_type,
      function_signature{
        token_provider.operator_plus_equal(),
        { parameter_declaration{ string_type, make_id_token("") } } },
      builtin_function_kind::string_operator_plus_equal },
    // Todo: clear could return a reference
    builtin_function_info{ // void clear()
                           void_type,
                           function_signature{ token_provider.clear() },
                           builtin_function_kind::string_clear },
    builtin_function_info{
      // string& insert(int position, string str)
      string_reference_type,
      function_signature{
        token_provider.insert(),
        { parameter_declaration{ int_type, make_id_token("") },
          parameter_declaration{ string_type, make_id_token("") } } },
      builtin_function_kind::string_insert_pos_str },
    builtin_function_info{
      // string& erase(int position)
      string_reference_type,
      function_signature{
        token_provider.erase_pos(),
        { parameter_declaration{ int_type, make_id_token("") } } },
      builtin_function_kind::string_erase_pos },
    builtin_function_info{
      // string& erase(int position, int count)
      string_reference_type,
      function_signature{
        token_provider.erase_pos_count(),
        { parameter_declaration{ int_type, make_id_token("") },
          parameter_declaration{ int_type, make_id_token("") } } },
      builtin_function_kind::string_erase_pos_count },
    builtin_function_info{
      // bool starts_with(string str)
      bool_type,
      function_signature{
        token_provider.starts_with(),
        { parameter_declaration{ string_type, make_id_token("") } } },
      builtin_function_kind::string_starts_with },
    builtin_function_info{
      // bool ends_with(string str)
      bool_type,
      function_signature{
        token_provider.ends_with(),
        { parameter_declaration{ string_type, make_id_token("") } } },
      builtin_function_kind::string_ends_with },
    builtin_function_info{
      // string& replace(int pos, int count, string str)
      string_reference_type,
      function_signature{
        token_provider.replace(),
        { parameter_declaration{ int_type, make_id_token("") },
          parameter_declaration{ int_type, make_id_token("") },
          parameter_declaration{ string_type, make_id_token("") } } },
      builtin_function_kind::string_replace_pos_count_str },
    builtin_function_info{
      // string substr(int pos)
      string_type,
      function_signature{
        token_provider.substr_pos(),
        { parameter_declaration{ int_type, make_id_token("") } } },
      builtin_function_kind::string_substr_pos },
    builtin_function_info{
      // string substr(int pos, int count)
      string_type,
      function_signature{
        token_provider.substr_pos_count(),
        { parameter_declaration{ int_type, make_id_token("") },
          parameter_declaration{ int_type, make_id_token("") } } },
      builtin_function_kind::string_substr_pos_count },
    // Todo: resize could return a reference
    builtin_function_info{
      // void resize(int new_size)
      void_type,
      function_signature{
        token_provider.resize_size(),
        { parameter_declaration{ int_type, make_id_token("") } } },
      builtin_function_kind::string_resize_newsize },
    // Todo: resize could return a reference
    builtin_function_info{
      // void resize(int new_size, string fill)
      void_type,
      function_signature{
        token_provider.resize_size_fill(),
        { parameter_declaration{ int_type, make_id_token("") },
          parameter_declaration{ string_type, make_id_token("") } } },
      builtin_function_kind::string_resize_newsize_fill },
    builtin_function_info{
      // int find(string str)
      int_type,
      function_signature{
        token_provider.find_str(),
        { parameter_declaration{ string_type, make_id_token("") } } },
      builtin_function_kind::string_find_str },
    builtin_function_info{
      // int find(string str, int pos)
      int_type,
      function_signature{
        token_provider.find_str_pos(),
        { parameter_declaration{ string_type, make_id_token("") },
          parameter_declaration{ int_type, make_id_token("") } } },
      builtin_function_kind::string_find_str_pos },
    builtin_function_info{
      // int find_not_of(string str)
      int_type,
      function_signature{
        token_provider.find_not_of_str(),
        { parameter_declaration{ string_type, make_id_token("") } } },
      builtin_function_kind::string_find_not_of_str },
    builtin_function_info{
      // int find_not_of(string str, int pos)
      int_type,
      function_signature{
        token_provider.find_not_of_str_pos(),
        { parameter_declaration{ string_type, make_id_token("") },
          parameter_declaration{ int_type, make_id_token("") } } },
      builtin_function_kind::string_find_not_of_str_pos },
    builtin_function_info{
      // int find_last(string str)
      int_type,
      function_signature{
        token_provider.find_last(),
        { parameter_declaration{ string_type, make_id_token("") } } },
      builtin_function_kind::string_find_last_str },
    builtin_function_info{
      // int find_last_not_of(string str)
      int_type,
      function_signature{
        token_provider.find_last_not_of(),
        { parameter_declaration{ string_type, make_id_token("") } } },
      builtin_function_kind::string_find_last_not_of_str },
    builtin_function_info{
      // bool contains(string str)
      bool_type,
      function_signature{
        token_provider.contains(),
        { parameter_declaration{ string_type, make_id_token("") } } },
      builtin_function_kind::string_contains },
    // Todo: lower should return reference
    builtin_function_info{ // void lower()
                           void_type,
                           function_signature{ token_provider.lower() },
                           builtin_function_kind::string_lower },
    builtin_function_info{ // string make_lower()
                           string_type,
                           function_signature{ token_provider.make_lower() },
                           builtin_function_kind::string_make_lower },
    // Todo: upper should return reference
    builtin_function_info{ // void upper()
                           void_type,
                           function_signature{ token_provider.upper() },
                           builtin_function_kind::string_upper },
    builtin_function_info{ // string make_upper()
                           string_type,
                           function_signature{ token_provider.make_upper() },
                           builtin_function_kind::string_make_upper }

  };

  add_type_member_functions(string_manipulator, functions);
}

type_builder builtin_sema_context::add_version_type()
{
  static const auto token = m_builtin_token_provider.version().name();
  return add_type(token);
}

void builtin_sema_context::add_version_member_functions(
  type_builder& string_manipulator)
{
  const auto& int_type = m_builtin_types->int_;
  const auto& bool_type = m_builtin_types->bool_;
  const auto& string_type = m_builtin_types->string;
  const auto& version_type = m_builtin_types->version;

  const auto token_provider = m_builtin_token_provider.version();

  const auto functions = {
    builtin_function_info{
      // version(int major)
      version_type,
      function_signature{
        token_provider.constructor_major(),
        { parameter_declaration{ int_type, make_id_token("") } } },
      builtin_function_kind::version_ctor_major },
    builtin_function_info{
      // version(int major, int minor)
      version_type,
      function_signature{
        token_provider.constructor_major_minor(),
        { parameter_declaration{ int_type, make_id_token("") },
          parameter_declaration{ int_type, make_id_token("") } } },
      builtin_function_kind::version_ctor_major_minor },
    builtin_function_info{
      // version(int major, int minor, int patch)
      version_type,
      function_signature{
        token_provider.constructor_major_minor_patch(),
        { parameter_declaration{ int_type, make_id_token("") },
          parameter_declaration{ int_type, make_id_token("") },
          parameter_declaration{ int_type, make_id_token("") } } },
      builtin_function_kind::version_ctor_major_minor_patch },
    builtin_function_info{
      // version(int major, int minor, int patch, int tweak)
      version_type,
      function_signature{
        token_provider.constructor_major_minor_patch_tweak(),
        { parameter_declaration{ int_type, make_id_token("") },
          parameter_declaration{ int_type, make_id_token("") },
          parameter_declaration{ int_type, make_id_token("") },
          parameter_declaration{ int_type, make_id_token("") } } },
      builtin_function_kind::version_ctor_major_minor_patch_tweak },
    builtin_function_info{
      // bool operator==(version)
      bool_type,
      function_signature{
        token_provider.operator_equal_equal(),
        { parameter_declaration{ version_type, make_id_token("") } } },
      builtin_function_kind::version_operator_equal_equal },
    builtin_function_info{
      // bool operator!=(version)
      bool_type,
      function_signature{
        token_provider.operator_not_equal(),
        { parameter_declaration{ version_type, make_id_token("") } } },
      builtin_function_kind::version_operator_not_equal },
    builtin_function_info{
      // bool operator<(version)
      bool_type,
      function_signature{
        token_provider.operator_less(),
        { parameter_declaration{ version_type, make_id_token("") } } },
      builtin_function_kind::version_operator_less },
    builtin_function_info{
      // bool operator<=(version)
      bool_type,
      function_signature{
        token_provider.operator_less_equal(),
        { parameter_declaration{ version_type, make_id_token("") } } },
      builtin_function_kind::version_operator_less_equal },
    builtin_function_info{
      // bool operator>(version)
      bool_type,
      function_signature{
        token_provider.operator_greater(),
        { parameter_declaration{ version_type, make_id_token("") } } },
      builtin_function_kind::version_operator_greater },
    builtin_function_info{
      // bool operator>=(version)
      bool_type,
      function_signature{
        token_provider.operator_greater_equal(),
        { parameter_declaration{ version_type, make_id_token("") } } },
      builtin_function_kind::version_operator_greater_equal },
    builtin_function_info{ // int major()
                           int_type,
                           function_signature{ token_provider.major_() },
                           builtin_function_kind::version_major },
    builtin_function_info{ // int minor()
                           int_type,
                           function_signature{ token_provider.minor_() },
                           builtin_function_kind::version_minor },
    builtin_function_info{ // int patch()
                           int_type,
                           function_signature{ token_provider.patch_() },
                           builtin_function_kind::version_patch },
    builtin_function_info{ // int tweak()
                           int_type,
                           function_signature{ token_provider.tweak_() },
                           builtin_function_kind::version_tweak },
    builtin_function_info{ // string to_string()
                           string_type,
                           function_signature{ token_provider.to_string() },
                           builtin_function_kind::version_to_string }
  };

  add_type_member_functions(string_manipulator, functions);
}

type_builder builtin_sema_context::add_project_type()
{
  static const auto token = m_builtin_token_provider.project().name();
  return add_type(token);
}

void builtin_sema_context::add_project_member_functions(
  type_builder& project_manipulator)
{
  const auto& string_type = m_builtin_types->string;
  const auto& project_type = m_builtin_types->project;
  const auto& library_type = m_builtin_types->library;
  const auto& executable_type = m_builtin_types->executable;

  const auto token_provider = m_builtin_token_provider.project();

  const auto string_token = make_token(token_type_t::kw_string, "string");
  const auto string_type_representation =
    ast::type_representation{ ast::qualified_name{ string_token } };

  auto generic_name = ast::type_representation::generic_type_name{
    { make_token(token_type_t::kw_list, "list"),
      make_token(token_type_t::less, "<"), string_token,
      make_token(token_type_t::greater, ">") },
    { string_type_representation }
  };
  const auto sources_list_type_name_representation =
    ast::type_representation{ generic_name };
  const auto& sources_type =
    get_or_create_generic_type(sources_list_type_name_representation);

  const auto functions = {
    builtin_function_info{
      // project(string name)
      project_type,
      function_signature{
        token_provider.name(),
        { parameter_declaration{ string_type, make_id_token("") } } },
      builtin_function_kind::project_ctor_name },
    builtin_function_info{ // string name()
                           string_type,
                           function_signature{ make_id_token("name") },
                           builtin_function_kind::project_name },
    builtin_function_info{
      // executable add_executable(string name, list<string> sources)
      executable_type,
      function_signature{
        token_provider.add_executable(),
        { parameter_declaration{ string_type, make_id_token("") },
          parameter_declaration{ sources_type, make_id_token("") } } },
      builtin_function_kind::project_add_executable },
    builtin_function_info{
      // library add_library(string name, list<string> sources)
      library_type,
      function_signature{
        token_provider.add_library(),
        { parameter_declaration{ string_type, make_id_token("") },
          parameter_declaration{ sources_type, make_id_token("") } } },
      builtin_function_kind::project_add_library }
  };

  add_type_member_functions(project_manipulator, functions);
}

type_builder builtin_sema_context::add_library_type()
{
  static const auto token = m_builtin_token_provider.library().name();
  return add_type(token);
}

void builtin_sema_context::add_library_member_functions(
  type_builder& project_manipulator)
{
  const auto& string_type = m_builtin_types->string;
  const auto& library_type = m_builtin_types->library;
  const auto& void_type = m_builtin_types->void_;

  const auto token_provider = m_builtin_token_provider.library();

  const auto functions = {
    builtin_function_info{ // string name()
                           string_type,
                           function_signature{ token_provider.method_name() },
                           builtin_function_kind::library_name },
    builtin_function_info{
      // void link_to(library target)
      void_type,
      function_signature{
        token_provider.link_to(),
        { parameter_declaration{ library_type, make_id_token("") } } },
      builtin_function_kind::library_link_to }
  };

  add_type_member_functions(project_manipulator, functions);
}

type_builder builtin_sema_context::add_executable_type()
{
  static const auto token = m_builtin_token_provider.executable().name();
  return add_type(token);
}

void builtin_sema_context::add_executable_member_functions(
  type_builder& project_manipulator)
{
  const auto& string_type = m_builtin_types->string;
  const auto& library_type = m_builtin_types->library;
  const auto& void_type = m_builtin_types->void_;

  const auto token_provider = m_builtin_token_provider.executable();

  const auto functions = {
    builtin_function_info{ // string name()
                           string_type,
                           function_signature{ token_provider.method_name() },
                           builtin_function_kind::executable_name },
    builtin_function_info{
      // void link_to(target target)
      void_type,
      function_signature{
        token_provider.link_to(),
        { parameter_declaration{ library_type, make_id_token("") } } },
      builtin_function_kind::executable_link_to }
  };

  add_type_member_functions(project_manipulator, functions);
}

const sema_type& builtin_sema_context::get_or_create_generic_type(
  const ast::type_representation& type_representation)
{
  if (const auto found = find_type(type_representation)) {
    return *found;
  }

  auto factory = sema_generic_type_factory{ *this,
                                            *this,
                                            m_type_factory,
                                            m_function_factory,
                                            m_context_factory,
                                            m_errors_observer,
                                            m_builtin_token_provider,
                                            *m_builtin_types,
                                            m_qualified_ctxs.types };
  return *factory.create_generic(type_representation);
}
type_builder builtin_sema_context::add_void_type()
{
  static const auto token = m_builtin_token_provider.void_().name();
  return add_type(token);
}

type_builder builtin_sema_context::add_type(lexer::token name_token)
{
  const auto name_representation =
    ast::type_representation{ ast::qualified_name{ name_token } };
  type_builder builder{ m_type_factory, m_function_factory, m_context_factory,
                        *this, name_representation };
  builder.build_builtin_and_register_in_context();
  return builder;
}

builtin_types_accessor builtin_sema_context::builtin_types() const
{
  return *m_builtin_types;
}

type_builder builtin_sema_context::add_option_type()
{
  static const auto token = m_builtin_token_provider.option().name();
  return add_type(token);
}

void builtin_sema_context::add_option_member_functions(
  type_builder& project_manipulator)
{
  const auto& string_type = m_builtin_types->string;
  const auto& bool_type = m_builtin_types->bool_;
  const auto& option_type = m_builtin_types->option;

  const auto token_provider = m_builtin_token_provider.option();

  const auto functions = {
    builtin_function_info{
      // option(string description)
      option_type,
      function_signature{
        token_provider.constructor_description(),
        { parameter_declaration{ string_type, make_id_token("") } } },
      builtin_function_kind::option_ctor_description },
    builtin_function_info{
      // option(string description, bool value)
      option_type,
      function_signature{
        token_provider.constructor_description_value(),
        { parameter_declaration{ string_type, make_id_token("") },
          parameter_declaration{ bool_type, make_id_token("") } } },
      builtin_function_kind::option_ctor_description_value },
    builtin_function_info{ // bool value()
                           bool_type,
                           function_signature{ token_provider.value() },
                           builtin_function_kind::option_value }
  };

  add_type_member_functions(project_manipulator, functions);
}

void builtin_sema_context::add_cmake_namespace_context()
{
  m_cmake_namespace_context =
    std::make_unique<builtin_cmake_namespace_context>(
      *this, m_qualified_ctxs, m_function_factory, m_builtin_token_provider,
      *m_builtin_types);
}
}
