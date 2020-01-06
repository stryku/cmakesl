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
  : builtin_context_base{ qualified_ctxs, factories, builtin_token_provider,
                          "cmake", &parent }
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
  const auto& system_info_type = m_types_accessor->system_info;
  const auto& list_of_strings_type =
    m_generics_creation_utils.list_of_strings();

  const auto& token_provider = m_builtin_tokens.cmake();

  const builtin_functions_t functions = {
    builtin_function_info{
      // void message(string)
      builtin_function_kind::cmake_minimum_required,
      void_type,
      token_provider.minimum_required(),
      { version_type },
    },
    builtin_function_info{ // void message(string)
                           builtin_function_kind::cmake_message,
                           void_type,
                           token_provider.message(),
                           { string_type } },
    builtin_function_info{ // void warning(string)
                           builtin_function_kind::cmake_warning,
                           void_type,
                           token_provider.warning(),
                           { string_type } },
    builtin_function_info{ // void error(string)
                           builtin_function_kind::cmake_error,
                           void_type,
                           token_provider.error(),
                           { string_type } },
    builtin_function_info{ // void fatal_error(string)
                           builtin_function_kind::cmake_fatal_error,
                           void_type,
                           token_provider.fatal_error(),
                           { string_type } },
    builtin_function_info{ // cxx_compiler_info get_cxx_compiler_info()
                           builtin_function_kind::cmake_get_cxx_compiler_info,
                           m_types_accessor->cxx_compiler_info,
                           token_provider.get_cxx_compiler_info() },
    builtin_function_info{ // void install(executable exe)
                           builtin_function_kind::cmake_install_executable,
                           void_type,
                           token_provider.install_executable(),
                           { executable_type } },
    builtin_function_info{
      // void install(executable exe, string destination)
      builtin_function_kind::cmake_install_executable_destination,
      void_type,
      token_provider.install_executable(),
      { executable_type, string_type } },
    builtin_function_info{ // void install(library lib)
                           builtin_function_kind::cmake_install_library,
                           void_type,
                           token_provider.install_library(),
                           { library_type } },
    builtin_function_info{
      // void install(library lib, string destination)
      builtin_function_kind::cmake_install_library_destination,
      void_type,
      token_provider.install_library(),
      { library_type, string_type } },
    builtin_function_info{
      // void enable_ctest()
      builtin_function_kind::cmake_enable_ctest,
      void_type,
      token_provider.enable_ctest(),
    },
    builtin_function_info{ // void add_test(executable exe)
                           builtin_function_kind::cmake_add_test,
                           void_type,
                           token_provider.add_test(),
                           { executable_type } },
    builtin_function_info{
      // string root_source_dir()
      builtin_function_kind::cmake_root_source_dir,
      string_type,
      token_provider.root_source_dir(),
    },
    builtin_function_info{
      // string current_binary_dir()
      builtin_function_kind::cmake_current_binary_dir,
      string_type,
      token_provider.current_binary_dir(),
    },
    builtin_function_info{
      // string current_source_dir()
      builtin_function_kind::cmake_current_source_dir,
      string_type,
      token_provider.current_source_dir(),
    },
    builtin_function_info{
      // void add_custom_command(list<string> command, string output)
      builtin_function_kind::cmake_add_custom_command,
      void_type,
      token_provider.add_custom_command(),
      { list_of_strings_type, string_type } },
    builtin_function_info{ // void make_directory(string dir)
                           builtin_function_kind::cmake_make_directory,
                           void_type,
                           token_provider.make_directory(),
                           { string_type } },
    builtin_function_info{
      // void set_old_style_variable(string name, string value)
      builtin_function_kind::cmake_set_old_style_variable,
      void_type,
      token_provider.set_old_style_variable(),
      { string_type, string_type } },
    builtin_function_info{
      // string get_old_style_variable(string name, string value)
      builtin_function_kind::cmake_get_old_style_variable,
      string_type,
      token_provider.get_old_style_variable(),
      { string_type },
    },
    builtin_function_info{ // system_info get_system_info()
                           builtin_function_kind::cmake_get_system_info,
                           system_info_type,
                           token_provider.get_system_info() },
    builtin_function_info{
      // void add_custom_target(string name, list<string> command)
      builtin_function_kind::cmake_add_custom_target,
      void_type,
      token_provider.add_custom_target(),
      { string_type, list_of_strings_type } },
    builtin_function_info{
      // string ctest_command()
      builtin_function_kind::cmake_ctest_command,
      string_type,
      token_provider.ctest_command(),
    }
  };

  auto factory = m_factories.function_factory();

  for (const auto& f : functions) {
    auto signature = make_function_signature(f);

    const auto& created_function =
      factory.create_builtin(*this, f.return_type, signature, f.kind);
    add_function(created_function);
    m_qualified_ctxs.functions.register_function(f.name, created_function,
                                                 /*exported=*/false);
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
  const auto& system_id_type = add_system_id_type();
  const auto& system_info_type = add_system_info_type(system_id_type);
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
    .option_ref = option_ref,
    .system_info = system_info_type
  };

  m_types_accessor =
    std::make_unique<cmake_namespace_types_accessor>(std::move(accessor));

  add_version_member_functions(version_manipulator);
  add_library_member_functions(library_manipulator);
  add_executable_member_functions(executable_manipulator);
  add_project_member_functions(project_manipulator);
  add_option_member_functions(option_manipulator);
}

const sema_type& builtin_cmake_namespace_context::add_cxx_compiler_id_type()
{
  static const auto token = m_builtin_tokens.cmake().cxx_compiler_id_name();
  const std::vector<lexer::token> enumerators{
    m_builtin_tokens.cmake().cxx_compiler_id_clang()
  };

  return add_enum_type(token, enumerators, m_builtin_types);
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

  return add_enum_type(token, enumerators, m_builtin_types);
}

const sema_type& builtin_cmake_namespace_context::add_visibility_type()
{
  static const auto token = m_builtin_tokens.cmake().visibility();
  const std::vector<lexer::token> enumerators{
    m_builtin_tokens.cmake().visibility_interface(),
    m_builtin_tokens.cmake().visibility_private(),
    m_builtin_tokens.cmake().visibility_public()
  };

  return add_enum_type(token, enumerators, m_builtin_types);
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
  return add_type_and_get_builder(token);
}

void builtin_cmake_namespace_context::add_version_member_functions(
  type_builder& string_manipulator)
{
  const auto& int_type = m_builtin_types.int_;
  const auto& bool_type = m_builtin_types.bool_;
  const auto& string_type = m_builtin_types.string;
  const auto& version_type = m_types_accessor->version;

  const auto token_provider = m_builtin_tokens.version();

  const builtin_functions_t functions = {
    builtin_function_info{ // version(int major)
                           builtin_function_kind::version_ctor_major,
                           version_type,
                           token_provider.constructor_major(),
                           { int_type } },
    builtin_function_info{ // version(int major, int minor)
                           builtin_function_kind::version_ctor_major_minor,
                           version_type,
                           token_provider.constructor_major_minor(),
                           { int_type, int_type } },
    builtin_function_info{
      // version(int major, int minor, int patch)
      builtin_function_kind::version_ctor_major_minor_patch,
      version_type,
      token_provider.constructor_major_minor_patch(),
      { int_type, int_type, int_type } },
    builtin_function_info{
      // version(int major, int minor, int patch, int tweak)
      builtin_function_kind::version_ctor_major_minor_patch_tweak,
      version_type,
      token_provider.constructor_major_minor_patch_tweak(),
      { int_type, int_type, int_type, int_type } },
    builtin_function_info{ // bool operator==(version)
                           builtin_function_kind::version_operator_equal_equal,
                           bool_type,
                           token_provider.operator_equal_equal(),
                           { version_type } },
    builtin_function_info{ // bool operator!=(version)
                           builtin_function_kind::version_operator_not_equal,
                           bool_type,
                           token_provider.operator_not_equal(),
                           { version_type } },
    builtin_function_info{ // bool operator<(version)
                           builtin_function_kind::version_operator_less,
                           bool_type,
                           token_provider.operator_less(),
                           { version_type } },
    builtin_function_info{ // bool operator<=(version)
                           builtin_function_kind::version_operator_less_equal,
                           bool_type,
                           token_provider.operator_less_equal(),
                           { version_type } },
    builtin_function_info{ // bool operator>(version)
                           builtin_function_kind::version_operator_greater,
                           bool_type,
                           token_provider.operator_greater(),
                           { version_type } },
    builtin_function_info{
      // bool operator>=(version)
      builtin_function_kind::version_operator_greater_equal,
      bool_type,
      token_provider.operator_greater_equal(),
      { version_type } },
    builtin_function_info{ // int major()
                           builtin_function_kind::version_major, int_type,
                           token_provider.major_() },
    builtin_function_info{ // int minor()
                           builtin_function_kind::version_minor, int_type,
                           token_provider.minor_() },
    builtin_function_info{ // int patch()
                           builtin_function_kind::version_patch, int_type,
                           token_provider.patch_() },
    builtin_function_info{ // int tweak()
                           builtin_function_kind::version_tweak, int_type,
                           token_provider.tweak_() },
    builtin_function_info{ // string to_string()
                           builtin_function_kind::version_to_string,
                           string_type, token_provider.to_string() }
  };

  add_type_member_functions(string_manipulator, functions);
}

type_builder builtin_cmake_namespace_context::add_library_type()
{
  static const auto token = m_builtin_tokens.library().name();
  return add_type_and_get_builder(token);
}

void builtin_cmake_namespace_context::add_library_member_functions(
  type_builder& project_manipulator)
{
  const auto& string_type = m_builtin_types.string;
  const auto& library_type = m_types_accessor->library;
  const auto& void_type = m_builtin_types.void_;
  const auto& list_of_strings_type =
    m_generics_creation_utils.list_of_strings();
  const auto& visibility_type = m_types_accessor->visibility;

  const auto token_provider = m_builtin_tokens.library();

  const builtin_functions_t functions = {
    builtin_function_info{ // string name()
                           builtin_function_kind::library_name, string_type,
                           token_provider.method_name() },
    builtin_function_info{ // void link_to(library target)
                           builtin_function_kind::library_link_to,
                           void_type,
                           token_provider.link_to(),
                           { library_type } },
    builtin_function_info{ // void link_to(library target, visibility v)
                           builtin_function_kind::library_link_to_visibility,
                           void_type,
                           token_provider.link_to_lib_visibility(),
                           { library_type, visibility_type } },
    builtin_function_info{ // void include_directories(list<string> dirs)
                           builtin_function_kind::library_include_directories,
                           void_type,
                           token_provider.include_directories(),
                           { list_of_strings_type } },
    builtin_function_info{
      // void include_directories(list<string> dirs, visibility v)
      builtin_function_kind::library_include_directories_visibility,
      void_type,
      token_provider.include_directories_dirs_visibility(),
      { list_of_strings_type, visibility_type } },
    builtin_function_info{
      // void compile_definitions(list<string> definitions)
      builtin_function_kind::library_compile_definitions,
      void_type,
      token_provider.compile_definitions(),
      { list_of_strings_type } },
    builtin_function_info{
      // void compile_definitions(list<string> definitions, visibility v)
      builtin_function_kind::library_compile_definitions_visibility,
      void_type,
      token_provider.compile_definitions_visibility(),
      { list_of_strings_type, visibility_type } },
    builtin_function_info{ // void compile_options(list<string> options)
                           builtin_function_kind::library_compile_options,
                           void_type,
                           token_provider.compile_options(),
                           { list_of_strings_type } },
    builtin_function_info{
      // void compile_options(list<string> options, visibility v)
      builtin_function_kind::library_compile_options_visibility,
      void_type,
      token_provider.compile_options_visibility(),
      { list_of_strings_type, visibility_type } }
  };

  add_type_member_functions(project_manipulator, functions);
}

type_builder builtin_cmake_namespace_context::add_executable_type()
{

  static const auto token = m_builtin_tokens.executable().name();
  return add_type_and_get_builder(token);
}

void builtin_cmake_namespace_context::add_executable_member_functions(
  type_builder& project_manipulator)
{
  const auto& string_type = m_builtin_types.string;
  const auto& library_type = m_types_accessor->library;
  const auto& void_type = m_builtin_types.void_;
  const auto& list_of_strings_type =
    m_generics_creation_utils.list_of_strings();
  const auto& visibility_type = m_types_accessor->visibility;

  const auto token_provider = m_builtin_tokens.executable();

  const builtin_functions_t functions = {
    builtin_function_info{ // string name()
                           builtin_function_kind::executable_name, string_type,
                           token_provider.method_name() },
    builtin_function_info{ // void link_to(target target)
                           builtin_function_kind::executable_link_to,
                           void_type,
                           token_provider.link_to(),
                           { library_type } },
    builtin_function_info{
      // void link_to(library target, visibility v)
      builtin_function_kind::executable_link_to_visibility,
      void_type,
      token_provider.link_to_lib_visibility(),
      { library_type, visibility_type } },
    builtin_function_info{
      // void include_directories(list<string> dirs)
      builtin_function_kind::executable_include_directories,
      void_type,
      token_provider.include_directories(),
      { list_of_strings_type } },
    builtin_function_info{
      // void include_directories(list<string> dirs, visibility v)
      builtin_function_kind::executable_include_directories_visibility,
      void_type,
      token_provider.include_directories_dirs_visibility(),
      { list_of_strings_type, visibility_type } },
    builtin_function_info{
      // void compile_definitions(list<string> definitions)
      builtin_function_kind::executable_compile_definitions,
      void_type,
      token_provider.compile_definitions(),
      { list_of_strings_type } },
    builtin_function_info{
      // void compile_definitions(list<string> definitions, visibility v)
      builtin_function_kind::executable_compile_definitions_visibility,
      void_type,
      token_provider.compile_definitions_visibility(),
      { list_of_strings_type, visibility_type } },
    builtin_function_info{ // void compile_options(list<string> options)
                           builtin_function_kind::executable_compile_options,
                           void_type,
                           token_provider.compile_options(),
                           { list_of_strings_type } },
    builtin_function_info{
      // void compile_options(list<string> options, visibility v)
      builtin_function_kind::executable_compile_options_visibility,
      void_type,
      token_provider.compile_options_visibility(),
      { list_of_strings_type, visibility_type } }
  };

  add_type_member_functions(project_manipulator, functions);
}

type_builder builtin_cmake_namespace_context::add_project_type()
{
  static const auto token = m_builtin_tokens.project().name();
  return add_type_and_get_builder(token);
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

  const builtin_functions_t functions = {
    builtin_function_info{ // project(string name)
                           builtin_function_kind::project_ctor_name,
                           project_type,
                           token_provider.constructor_name(),
                           { string_type } },
    builtin_function_info{ // string name()
                           builtin_function_kind::project_name, string_type,
                           token_provider.name() },
    builtin_function_info{
      // executable add_executable(string name, list<string> sources)
      builtin_function_kind::project_add_executable,
      executable_type,
      token_provider.add_executable(),
      { string_type, sources_type } },
    builtin_function_info{
      // library add_library(string name, list<string> sources)
      builtin_function_kind::project_add_library,
      library_type,
      token_provider.add_library(),
      { string_type, sources_type } },
    builtin_function_info{ // library find_library(string name)
                           builtin_function_kind::project_find_library,
                           library_type,
                           token_provider.find_library(),
                           { string_type } }
  };

  add_type_member_functions(project_manipulator, functions);
}

type_builder builtin_cmake_namespace_context::add_option_type()
{
  static const auto token = m_builtin_tokens.option().name();
  return add_type_and_get_builder(token);
}

void builtin_cmake_namespace_context::add_option_member_functions(
  type_builder& project_manipulator)
{
  const auto& string_type = m_builtin_types.string;
  const auto& bool_type = m_builtin_types.bool_;
  const auto& option_type = m_types_accessor->option;

  const auto token_provider = m_builtin_tokens.option();

  const builtin_functions_t functions = {
    builtin_function_info{ // option(string name, string description)
                           builtin_function_kind::option_ctor_description,
                           option_type,
                           token_provider.constructor_description(),
                           { string_type, string_type } },
    builtin_function_info{
      // option(string name, string description, bool value)
      builtin_function_kind::option_ctor_description_value,
      option_type,
      token_provider.constructor_description_value(),
      { string_type, string_type, bool_type } },
    builtin_function_info{ // bool value()
                           builtin_function_kind::option_value, bool_type,
                           token_provider.value() }
  };

  add_type_member_functions(project_manipulator, functions);
}

const sema_type& builtin_cmake_namespace_context::add_system_id_type()
{
  static const auto token = m_builtin_tokens.cmake().system_id_name();
  const std::vector<lexer::token> enumerators{
    m_builtin_tokens.cmake().system_id_windows(),
    m_builtin_tokens.cmake().system_id_unix(),
  };

  return add_enum_type(token, enumerators, m_builtin_types);
}

const sema_type& builtin_cmake_namespace_context::add_system_info_type(
  const sema_type& system_id)
{

  static const auto token = m_builtin_tokens.cmake().system_info_name();
  const auto name_representation =
    ast::type_representation{ ast::qualified_name{ token } };
  type_builder builder{ m_factories, m_qualified_ctxs.types, *this,
                        name_representation };

  const auto members = { builtin_variable_info{
    m_builtin_tokens.cmake().system_info_id(), system_id } };

  for (const auto& member : members) {
    const auto id_index = identifiers_index_provider::get_next();
    member_info info{ member.name, member.type, id_index };
    builder.with_member(info);
  }

  builder.build_builtin_and_register_in_context();
  return builder.built_type().ty;
}
}
