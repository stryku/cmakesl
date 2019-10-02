#include "sema/builtin_cmake_namespace_context.hpp"

#include "lexer/token.hpp"
#include "sema/builtin_sema_function.hpp"
#include "sema/builtin_token_provider.hpp"
#include "sema/cmake_namespace_types_accessor.hpp"
#include "sema/enum_creator.hpp"
#include "sema/factories.hpp"
#include "sema/factories_provider.hpp"
#include "sema/functions_context.hpp"
#include "sema/generic_type_creation_utils.hpp"
#include "sema/identifiers_index_provider.hpp"
#include "sema/qualified_contextes_refs.hpp"
#include "sema/type_builder.hpp"

#include <generated/builtin_token_providers.hpp>

namespace {
const auto param_token =
  cmsl::lexer::make_token(cmsl::lexer::token_type::identifier, "");
}

namespace cmsl::sema {

builtin_cmake_namespace_context::builtin_cmake_namespace_context(
  const sema_context& parent, qualified_contextes_refs& qualified_ctxs,
  factories_provider& factories,
  const builtin_token_provider& builtin_token_provider,
  const builtin_types_accessor& builtin_types,
  generic_type_creation_utils& generics_creation_utils)
  : sema_context_impl{ "cmake", &parent }
  , m_qualified_ctxs{ qualified_ctxs }
  , m_factories{ factories }
  , m_builtin_tokens{ builtin_token_provider }
  , m_builtin_types{ builtin_types }
  , m_generics_creation_utils{ generics_creation_utils }
{
  auto guard = m_qualified_ctxs.all_qualified_ctxs_guard(
    m_builtin_tokens.cmake().name(), /*exported=*/false);

  add_types();
  add_functions();
  add_identifiers();
}

builtin_cmake_namespace_context::~builtin_cmake_namespace_context()
{
}

void builtin_cmake_namespace_context::add_functions()
{
  const auto& string_type = m_builtin_types.string;
  const auto& void_type = m_builtin_types.void_;
  const auto& executable_type = m_types_accessor->executable;
  const auto& library_type = m_types_accessor->library;
  const auto& version_type = m_types_accessor->version;

  const auto& token_provider = m_builtin_tokens.cmake();

  const auto functions = {
    builtin_function_info{
      // void message(string)
      void_type,
      function_signature{
        token_provider.minimum_required(),
        { parameter_declaration{ version_type, param_token } } },
      builtin_function_kind::cmake_minimum_required },
    builtin_function_info{
      // void message(string)
      void_type,
      function_signature{
        token_provider.message(),
        { parameter_declaration{ string_type, param_token } } },
      builtin_function_kind::cmake_message },
    builtin_function_info{
      // void warning(string)
      void_type,
      function_signature{
        token_provider.warning(),
        { parameter_declaration{ string_type, param_token } } },
      builtin_function_kind::cmake_warning },
    builtin_function_info{
      // void error(string)
      void_type,
      function_signature{
        token_provider.error(),
        { parameter_declaration{ string_type, param_token } } },
      builtin_function_kind::cmake_error },
    builtin_function_info{
      // void fatal_error(string)
      void_type,
      function_signature{
        token_provider.fatal_error(),
        { parameter_declaration{ string_type, param_token } } },
      builtin_function_kind::cmake_fatal_error },
    builtin_function_info{
      // cxx_compiler_info get_cxx_compiler_info()
      m_types_accessor->cxx_compiler_info,
      function_signature{ token_provider.get_cxx_compiler_info() },
      builtin_function_kind::cmake_get_cxx_compiler_info },
    builtin_function_info{
      // void install(executable exe)
      void_type,
      function_signature{
        token_provider.install_executable(),
        { parameter_declaration{ executable_type, param_token } } },
      builtin_function_kind::cmake_install_executable },
    builtin_function_info{
      // void install(library lib)
      void_type,
      function_signature{
        token_provider.install_library(),
        { parameter_declaration{ library_type, param_token } } },
      builtin_function_kind::cmake_install_library },
    builtin_function_info{ // void enable_ctest()
                           void_type,
                           function_signature{ token_provider.enable_ctest() },
                           builtin_function_kind::cmake_enable_ctest },
    builtin_function_info{
      // void add_test(executable exe)
      void_type,
      function_signature{
        token_provider.add_test(),
        { parameter_declaration{ executable_type, param_token } } },
      builtin_function_kind::cmake_add_test }
  };

  auto factory = m_factories.function_factory();

  for (const auto& f : functions) {
    const auto& created_function =
      factory.create_builtin(*this, f.return_type, f.signature, f.kind);
    add_function(created_function);
    m_qualified_ctxs.functions.register_function(
      f.signature.name, created_function, /*exported=*/false);
  }
}

void builtin_cmake_namespace_context::add_identifiers()
{
  const auto& list_of_string_type =
    m_generics_creation_utils.list_of_strings();
  const auto& cxx_standard_value_type = m_types_accessor->cxx_standard_value;

  const auto identifiers = {
    builtin_variable_info{ m_builtin_tokens.cmake().module_path(),
                           list_of_string_type, "CMAKE_MODULE_PATH" },
    builtin_variable_info{ m_builtin_tokens.cmake().cxx_standard(),
                           cxx_standard_value_type, "CMAKE_CXX_STANDARD" }
  };

  for (const auto& identifier : identifiers) {
    const auto identifier_index = identifiers_index_provider::get_next();
    const auto id_info = identifier_info{ identifier.type, identifier_index };
    const auto builtin_id_info =
      builtin_identifier_info{ id_info, identifier.cmake_variable_name };

    m_qualified_ctxs.ids.register_identifier(identifier.name, id_info,
                                             /*exported=*/false);
    m_builtin_identifiers_info.emplace_back(builtin_id_info);
  }
}

const std::vector<builtin_identifier_info>&
builtin_cmake_namespace_context::builtin_identifiers_info() const
{
  return m_builtin_identifiers_info;
}

void builtin_cmake_namespace_context::add_types()
{
  const auto& cxx_compiler_id_type = add_cxx_compiler_id_type();
  const auto& cxx_standard_value_type = add_cxx_standard_value_type();
  const auto& visibility_type = add_visibility_type();
  auto cxx_compiler_info_manipulator =
    add_cxx_compiler_info_type(cxx_compiler_id_type);
  auto version_manipulator = add_version_type();
  auto library_manipulator = add_library_type();
  auto executable_manipulator = add_executable_type();
  auto project_manipulator = add_project_type();
  auto option_manipulator = add_option_type();

  const auto& [cxx_compiler_info, cxx_compiler_info_ref] =
    cxx_compiler_info_manipulator.built_type();
  const auto& [version, version_ref] = version_manipulator.built_type();
  const auto& [library, library_ref] = library_manipulator.built_type();
  const auto& [executable, executable_ref] =
    executable_manipulator.built_type();
  const auto& [project, project_ref] = project_manipulator.built_type();
  const auto& [option, option_ref] = option_manipulator.built_type();

  cmake_namespace_types_accessor accessor{
    .cxx_compiler_id = cxx_compiler_id_type,
    .cxx_compiler_id_ref = *find_reference_for(cxx_compiler_id_type),
    .cxx_compiler_info = cxx_compiler_info,
    .cxx_compiler_info_ref = cxx_compiler_info_ref,
    .cxx_standard_value = cxx_standard_value_type,
    .cxx_standard_value_ref = *find_reference_for(cxx_standard_value_type),
    .visibility = visibility_type,
    .visibility_ref = *find_reference_for(cxx_compiler_id_type),
    .version = version,
    .version_ref = version_ref,
    .library = library,
    .library_ref = library_ref,
    .executable = executable,
    .executable_ref = executable_ref,
    .project = project,
    .project_ref = project_ref,
    .option = option,
    .option_ref = option_ref
  };

  m_types_accessor =
    std::make_unique<cmake_namespace_types_accessor>(std::move(accessor));

  add_version_member_functions(version_manipulator);
  add_library_member_functions(library_manipulator);
  add_executable_member_functions(executable_manipulator);
  add_project_member_functions(project_manipulator);
  add_option_member_functions(option_manipulator);
}

type_builder builtin_cmake_namespace_context::add_type(lexer::token name_token)
{
  const auto name_representation =
    ast::type_representation{ ast::qualified_name{ name_token } };
  type_builder builder{ m_factories, m_qualified_ctxs.types, *this,
                        name_representation };
  builder.build_builtin_and_register_in_context();
  return builder;
}

const sema_type& builtin_cmake_namespace_context::add_cxx_compiler_id_type()
{
  static const auto token = m_builtin_tokens.cmake().cxx_compiler_id_name();
  const std::vector<lexer::token> enumerators{
    m_builtin_tokens.cmake().cxx_compiler_id_clang()
  };

  enum_creator creator{ m_factories, m_qualified_ctxs.types, *this,
                        m_builtin_types };

  const auto& type =
    creator.create(token, enumerators, sema_type::flags::builtin);

  {
    auto guard = m_qualified_ctxs.enums_ctx_guard(token, /*exported=*/false);

    unsigned value{};

    for (const auto& enumerator : enumerators) {
      const auto enum_value_index = identifiers_index_provider::get_next();
      const auto enum_value = value++;
      m_qualified_ctxs.enums.register_identifier(
        enumerator,
        enum_values_context::enum_value_info{ type, enum_value,
                                              enum_value_index },
        /*exported=*/false);
    }
  }

  return type;
}

const sema_type& builtin_cmake_namespace_context::add_cxx_standard_value_type()
{
  static const auto token = m_builtin_tokens.cmake().cxx_standard_value_name();
  const std::vector<lexer::token> enumerators{
    m_builtin_tokens.cmake().cxx_standard_value_cpp_11(),
    m_builtin_tokens.cmake().cxx_standard_value_cpp_14(),
    m_builtin_tokens.cmake().cxx_standard_value_cpp_17(),
    m_builtin_tokens.cmake().cxx_standard_value_cpp_20()
  };

  enum_creator creator{ m_factories, m_qualified_ctxs.types, *this,
                        m_builtin_types };

  const auto& type =
    creator.create(token, enumerators, sema_type::flags::builtin);

  {
    auto guard = m_qualified_ctxs.enums_ctx_guard(token, /*exported=*/false);

    unsigned value{};

    for (const auto& enumerator : enumerators) {
      const auto enum_value_index = identifiers_index_provider::get_next();
      const auto enum_value = value++;
      m_qualified_ctxs.enums.register_identifier(
        enumerator,
        enum_values_context::enum_value_info{ type, enum_value,
                                              enum_value_index },
        /*exported=*/false);
    }
  }

  return type;
}

const sema_type& builtin_cmake_namespace_context::add_visibility_type()
{
  static const auto token = m_builtin_tokens.cmake().visibility();
  const std::vector<lexer::token> enumerators{
    m_builtin_tokens.cmake().visibility_interface(),
    m_builtin_tokens.cmake().visibility_private(),
    m_builtin_tokens.cmake().visibility_public()
  };

  enum_creator creator{ m_factories, m_qualified_ctxs.types, *this,
                        m_builtin_types };

  const auto& type =
    creator.create(token, enumerators, sema_type::flags::builtin);

  {
    auto guard = m_qualified_ctxs.enums_ctx_guard(token, /*exported=*/false);

    unsigned value{};

    for (const auto& enumerator : enumerators) {
      const auto enum_value_index = identifiers_index_provider::get_next();
      const auto enum_value = value++;
      m_qualified_ctxs.enums.register_identifier(
        enumerator,
        enum_values_context::enum_value_info{ type, enum_value,
                                              enum_value_index },
        /*exported=*/false);
    }
  }

  return type;
}

type_builder builtin_cmake_namespace_context::add_cxx_compiler_info_type(
  const sema_type& cxx_compiler_id)
{
  static const auto token = m_builtin_tokens.cmake().cxx_compiler_info_name();
  const auto name_representation =
    ast::type_representation{ ast::qualified_name{ token } };
  type_builder builder{ m_factories, m_qualified_ctxs.types, *this,
                        name_representation };

  const auto members = { builtin_variable_info{
    m_builtin_tokens.cmake().cxx_compiler_info_id(), cxx_compiler_id } };

  for (const auto& member : members) {
    const auto id_index = identifiers_index_provider::get_next();
    member_info info{ member.name, member.type, id_index };
    builder.with_member(info);
  }

  builder.build_builtin_and_register_in_context();
  return builder;
}

const cmake_namespace_types_accessor&
builtin_cmake_namespace_context::types_accessor() const
{
  return *m_types_accessor;
}

type_builder builtin_cmake_namespace_context::add_version_type()
{
  static const auto token = m_builtin_tokens.version().name();
  return add_type(token);
}

void builtin_cmake_namespace_context::add_version_member_functions(
  type_builder& string_manipulator)
{
  const auto& int_type = m_builtin_types.int_;
  const auto& bool_type = m_builtin_types.bool_;
  const auto& string_type = m_builtin_types.string;
  const auto& version_type = m_types_accessor->version;

  const auto token_provider = m_builtin_tokens.version();

  const auto functions = {
    builtin_function_info{
      // version(int major)
      version_type,
      function_signature{ token_provider.constructor_major(),
                          { parameter_declaration{ int_type, param_token } } },
      builtin_function_kind::version_ctor_major },
    builtin_function_info{
      // version(int major, int minor)
      version_type,
      function_signature{ token_provider.constructor_major_minor(),
                          { parameter_declaration{ int_type, param_token },
                            parameter_declaration{ int_type, param_token } } },
      builtin_function_kind::version_ctor_major_minor },
    builtin_function_info{
      // version(int major, int minor, int patch)
      version_type,
      function_signature{ token_provider.constructor_major_minor_patch(),
                          { parameter_declaration{ int_type, param_token },
                            parameter_declaration{ int_type, param_token },
                            parameter_declaration{ int_type, param_token } } },
      builtin_function_kind::version_ctor_major_minor_patch },
    builtin_function_info{
      // version(int major, int minor, int patch, int tweak)
      version_type,
      function_signature{ token_provider.constructor_major_minor_patch_tweak(),
                          { parameter_declaration{ int_type, param_token },
                            parameter_declaration{ int_type, param_token },
                            parameter_declaration{ int_type, param_token },
                            parameter_declaration{ int_type, param_token } } },
      builtin_function_kind::version_ctor_major_minor_patch_tweak },
    builtin_function_info{
      // bool operator==(version)
      bool_type,
      function_signature{
        token_provider.operator_equal_equal(),
        { parameter_declaration{ version_type, param_token } } },
      builtin_function_kind::version_operator_equal_equal },
    builtin_function_info{
      // bool operator!=(version)
      bool_type,
      function_signature{
        token_provider.operator_not_equal(),
        { parameter_declaration{ version_type, param_token } } },
      builtin_function_kind::version_operator_not_equal },
    builtin_function_info{
      // bool operator<(version)
      bool_type,
      function_signature{
        token_provider.operator_less(),
        { parameter_declaration{ version_type, param_token } } },
      builtin_function_kind::version_operator_less },
    builtin_function_info{
      // bool operator<=(version)
      bool_type,
      function_signature{
        token_provider.operator_less_equal(),
        { parameter_declaration{ version_type, param_token } } },
      builtin_function_kind::version_operator_less_equal },
    builtin_function_info{
      // bool operator>(version)
      bool_type,
      function_signature{
        token_provider.operator_greater(),
        { parameter_declaration{ version_type, param_token } } },
      builtin_function_kind::version_operator_greater },
    builtin_function_info{
      // bool operator>=(version)
      bool_type,
      function_signature{
        token_provider.operator_greater_equal(),
        { parameter_declaration{ version_type, param_token } } },
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

type_builder builtin_cmake_namespace_context::add_library_type()
{
  static const auto token = m_builtin_tokens.library().name();
  return add_type(token);
}

void builtin_cmake_namespace_context::add_library_member_functions(
  type_builder& project_manipulator)
{
  const auto& string_type = m_builtin_types.string;
  const auto& library_type = m_types_accessor->library;
  const auto& void_type = m_builtin_types.void_;
  const auto& directories_type = m_generics_creation_utils.list_of_strings();
  const auto& visibility_type = m_types_accessor->visibility;

  const auto token_provider = m_builtin_tokens.library();

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
        { parameter_declaration{ library_type, param_token } } },
      builtin_function_kind::library_link_to },
    builtin_function_info{
      // void link_to(library target, visibility v)
      void_type,
      function_signature{
        token_provider.link_to(),
        { parameter_declaration{ library_type, param_token },
          parameter_declaration{ visibility_type, param_token } } },
      builtin_function_kind::library_link_to_visibility },
    builtin_function_info{
      // void include_directories(list<string> dirs)
      void_type,
      function_signature{
        token_provider.include_directories(),
        { parameter_declaration{ directories_type, param_token } } },
      builtin_function_kind::library_include_directories },
    builtin_function_info{
      // void include_directories(list<string> dirs, visibility v)
      void_type,
      function_signature{
        token_provider.include_directories(),
        { parameter_declaration{ directories_type, param_token },
          parameter_declaration{ visibility_type, param_token } } },
      builtin_function_kind::library_include_directories_visibility }
  };

  add_type_member_functions(project_manipulator, functions);
}

type_builder builtin_cmake_namespace_context::add_executable_type()
{

  static const auto token = m_builtin_tokens.executable().name();
  return add_type(token);
}

void builtin_cmake_namespace_context::add_executable_member_functions(
  type_builder& project_manipulator)
{
  const auto& string_type = m_builtin_types.string;
  const auto& library_type = m_types_accessor->library;
  const auto& void_type = m_builtin_types.void_;
  const auto& directories_type = m_generics_creation_utils.list_of_strings();
  const auto& visibility_type = m_types_accessor->visibility;

  const auto token_provider = m_builtin_tokens.executable();

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
        { parameter_declaration{ library_type, param_token } } },
      builtin_function_kind::executable_link_to },
    builtin_function_info{
      // void link_to(library target, visibility v)
      void_type,
      function_signature{
        token_provider.link_to(),
        { parameter_declaration{ library_type, param_token },
          parameter_declaration{ visibility_type, param_token } } },
      builtin_function_kind::executable_link_to_visibility },
    builtin_function_info{
      // void include_directories(list<string> dirs)
      void_type,
      function_signature{
        token_provider.include_directories(),
        { parameter_declaration{ directories_type, param_token } } },
      builtin_function_kind::executable_include_directories },
    builtin_function_info{
      // void include_directories(list<string> dirs, visibility v)
      void_type,
      function_signature{
        token_provider.include_directories(),
        { parameter_declaration{ directories_type, param_token },
          parameter_declaration{ visibility_type, param_token } } },
      builtin_function_kind::executable_include_directories_visibility }
  };

  add_type_member_functions(project_manipulator, functions);
}

type_builder builtin_cmake_namespace_context::add_project_type()
{
  static const auto token = m_builtin_tokens.project().name();
  return add_type(token);
}

void builtin_cmake_namespace_context::add_project_member_functions(
  type_builder& project_manipulator)
{
  const auto& string_type = m_builtin_types.string;
  const auto& project_type = m_types_accessor->project;
  const auto& library_type = m_types_accessor->library;
  const auto& executable_type = m_types_accessor->executable;

  const auto token_provider = m_builtin_tokens.project();
  const auto& sources_type = m_generics_creation_utils.list_of_strings();

  const auto functions = {
    builtin_function_info{
      // project(string name)
      project_type,
      function_signature{
        token_provider.name(),
        { parameter_declaration{ string_type, param_token } } },
      builtin_function_kind::project_ctor_name },
    builtin_function_info{ // string name()
                           string_type, function_signature{ param_token },
                           builtin_function_kind::project_name },
    builtin_function_info{
      // executable add_executable(string name, list<string> sources)
      executable_type,
      function_signature{
        token_provider.add_executable(),
        { parameter_declaration{ string_type, param_token },
          parameter_declaration{ sources_type, param_token } } },
      builtin_function_kind::project_add_executable },
    builtin_function_info{
      // library add_library(string name, list<string> sources)
      library_type,
      function_signature{
        token_provider.add_library(),
        { parameter_declaration{ string_type, param_token },
          parameter_declaration{ sources_type, param_token } } },
      builtin_function_kind::project_add_library }
  };

  add_type_member_functions(project_manipulator, functions);
}

type_builder builtin_cmake_namespace_context::add_option_type()
{
  static const auto token = m_builtin_tokens.option().name();
  return add_type(token);
}

void builtin_cmake_namespace_context::add_option_member_functions(
  type_builder& project_manipulator)
{
  const auto& string_type = m_builtin_types.string;
  const auto& bool_type = m_builtin_types.bool_;
  const auto& option_type = m_types_accessor->option;

  const auto token_provider = m_builtin_tokens.option();

  const auto functions = {
    builtin_function_info{
      // option(string description)
      option_type,
      function_signature{
        token_provider.constructor_description(),
        { parameter_declaration{ string_type, param_token } } },
      builtin_function_kind::option_ctor_description },
    builtin_function_info{
      // option(string description, bool value)
      option_type,
      function_signature{
        token_provider.constructor_description_value(),
        { parameter_declaration{ string_type, param_token },
          parameter_declaration{ bool_type, param_token } } },
      builtin_function_kind::option_ctor_description_value },
    builtin_function_info{ // bool value()
                           bool_type,
                           function_signature{ token_provider.value() },
                           builtin_function_kind::option_value }
  };

  add_type_member_functions(project_manipulator, functions);
}

template <typename Functions>
void builtin_cmake_namespace_context::add_type_member_functions(
  type_builder& manipulator, Functions&& functions)
{
  for (const auto& function : functions) {
    manipulator.with_builtin_function(
      function.return_type, std::move(function.signature), function.kind);
  }
}
}
