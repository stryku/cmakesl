#include "exec/global_executor.hpp"

#include "exec/compiled_source.hpp"
#include "exec/execution.hpp"
#include "exec/source_compiler.hpp"
#include "sema/builtin_sema_context.hpp"
#include "sema/builtin_token_provider.hpp"
#include "sema/enum_values_context.hpp"
#include "sema/functions_context.hpp"
#include "sema/identifiers_context.hpp"
#include "sema/qualified_contextes_refs.hpp"
#include "sema/types_context.hpp"
#include "sema/user_sema_function.hpp"

#include "cmake_facade.hpp"

#include <errors/error.hpp>
#include <fstream>
#include <iterator>

namespace cmsl::exec {
global_executor::directory_guard::directory_guard(
  facade::cmake_facade& cmake_facade, const std::string& dir)
  : m_cmake_facade{ cmake_facade }
{
  m_cmake_facade.go_into_subdirectory(dir);
}

global_executor::directory_guard::~directory_guard()
{
  m_cmake_facade.go_directory_up();
}

global_executor::global_executor(const std::string& root_path,
                                 facade::cmake_facade& cmake_facade)
  : m_root_path{ root_path }
  , m_cmake_facade{ cmake_facade }
  , m_builtin_qualified_contexts{ create_qualified_contextes() }
  , m_builtin_tokens{ std::make_unique<sema::builtin_token_provider>("") }
  , m_builtin_context{ create_builtin_context() }
  , m_static_variables{ m_cmake_facade, m_builtin_context->builtin_types(),
                        *this }
{
  m_cmake_facade.go_into_subdirectory(m_root_path);
}

global_executor::~global_executor() = default;

int global_executor::execute(std::string source)
{
  auto contextes = m_builtin_qualified_contexts.clone();
  auto refs = sema::qualified_contextes_refs{ contextes };
  source_compiler compiler{
    m_errors_observer,  m_factories,      *this, *this, refs,
    *m_builtin_context, *m_builtin_tokens
  };
  const auto source_path = m_root_path + "/CMakeLists.cmsl";
  auto path_copy = source_path;
  const auto src_view = source_view{ source_path, source };

  m_sources.emplace_back(std::move(source));
  auto compiled = compiler.compile(src_view);
  if (!compiled) {
    raise_unsuccessful_compilation_error(source_path);
    return -1;
  }

  const auto& sema_tree = compiled->sema_tree();
  m_sema_trees.emplace(std::move(path_copy), sema_tree);

  const auto builtin_types = compiled->builtin_types();

  const auto main_function = compiled->get_main();
  if (!main_function) {
    raise_no_main_function_error(m_root_path);
    return -2;
  }

  const auto& global_context = compiled->get_global_context();

  const auto casted =
    dynamic_cast<const sema::user_sema_function*>(main_function);

  const auto builtin_identifiers_info =
    m_builtin_context->builtin_identifiers_info();

  m_static_variables.initialize_builtin_variables(builtin_identifiers_info);

  execution e{ m_cmake_facade, compiled->builtin_types(), m_static_variables };

  const auto translation_unit =
    dynamic_cast<const sema::translation_unit_node*>(&compiled->sema_tree());
  e.initialize_static_variables(*translation_unit, m_static_variables);

  m_compiled_sources.emplace_back(std::move(compiled));

  inst::instances_holder instances{ builtin_types };
  auto main_result = e.call(*casted, {}, instances);

  if (m_cmake_facade.did_fatal_error_occure()) {
    return -1;
  }

  return main_result->value_cref().get_int();
}

const sema::sema_function* global_executor::handle_add_subdirectory(
  cmsl::string_view name,
  const std::vector<std::unique_ptr<sema::expression_node>>&)
{
  directory_guard dg{ m_cmake_facade, std::string{ name } };

  auto path = m_cmake_facade.current_directory() + "/CMakeLists.cmsl";
  std::ifstream file{ path };
  if (!file.is_open()) {
    // Todo: script not found
    return nullptr;
  }
  cmsl::string_view path_view = path;
  auto path_copy = path;
  m_paths.emplace_back(std::move(path));

  std::string source(std::istreambuf_iterator<char>{ file }, {});
  const auto src_view = source_view{ path_view, source };

  m_sources.emplace_back(std::move(source));

  auto contextes = m_builtin_qualified_contexts.clone();
  auto refs = sema::qualified_contextes_refs{ contextes };

  source_compiler compiler{
    m_errors_observer,  m_factories,      *this, *this, refs,
    *m_builtin_context, *m_builtin_tokens
  };

  auto compiled = compiler.compile(src_view);
  const auto& sema_tree = compiled->sema_tree();
  m_sema_trees.emplace(std::move(path_copy), sema_tree);

  const auto main_function = compiled->get_main();
  if (!main_function) {
    raise_no_main_function_error(path_view);
    return nullptr;
  }

  m_compiled_sources.emplace_back(std::move(compiled));

  // Todo: handle not matching params.
  return main_function;
}

void global_executor::raise_no_main_function_error(
  const cmsl::string_view& path)
{
  errors::error err;
  err.source_path = path;
  err.message = "main function not found";
  err.type = errors::error_type::error;
  err.range = source_range{ source_location{}, source_location{} };
  m_errors_observer.notify_error(err);
}

void global_executor::raise_unsuccessful_compilation_error(
  const cmsl::string_view& path)
{
  errors::error err;
  err.source_path = path;
  err.message = "parsing failed, can not execute";
  err.type = errors::error_type::error;
  err.range = source_range{ source_location{}, source_location{} };
  m_errors_observer.notify_error(err);
}

sema::qualified_contextes global_executor::create_qualified_contextes() const
{
  return sema::qualified_contextes{
    std::make_unique<sema::enum_values_context_impl>(),
    std::make_unique<sema::functions_context_impl>(),
    std::make_unique<sema::identifiers_context_impl>(),
    std::make_unique<sema::types_context_impl>()
  };
}

std::unique_ptr<sema::builtin_sema_context>
global_executor::create_builtin_context()
{
  auto refs = sema::qualified_contextes_refs{ m_builtin_qualified_contexts };
  return std::make_unique<sema::builtin_sema_context>(
    m_factories, m_errors_observer, *m_builtin_tokens, refs);
}

std::unique_ptr<sema::qualified_contextes> global_executor::handle_import(
  cmsl::string_view path)
{
  auto import_path = build_full_import_path(path);

  if (const auto found = m_exported_qualified_contextes.find(import_path);
      found != std::cend(m_exported_qualified_contextes)) {
    const auto& ctxs = found->second;
    return std::make_unique<sema::qualified_contextes>(ctxs.clone());
  }

  std::ifstream file{ import_path };
  if (!file.is_open()) {
    // Todo: file not found
    return nullptr;
  }
  cmsl::string_view path_view = import_path;
  auto path_copy = import_path;
  m_paths.emplace_back(std::move(import_path));

  std::string source(std::istreambuf_iterator<char>{ file }, {});
  const auto src_view = source_view{ path_view, source };

  m_sources.emplace_back(std::move(source));

  auto contextes = m_builtin_qualified_contexts.clone();
  auto refs = sema::qualified_contextes_refs{ contextes };

  source_compiler compiler{
    m_errors_observer,  m_factories,      *this, *this, refs,
    *m_builtin_context, *m_builtin_tokens
  };

  auto compiled = compiler.compile(src_view);
  if (!compiled) {
    // Todo: compilation failed
    return nullptr;
  }

  const auto& sema_tree = compiled->sema_tree();

  auto exported_stuff = contextes.collect_exported_stuff();

  m_exported_qualified_contextes.emplace(path_copy, exported_stuff.clone());
  m_sema_trees.emplace(std::move(path_copy), sema_tree);
  m_compiled_sources.emplace_back(std::move(compiled));

  return std::make_unique<sema::qualified_contextes>(
    std::move(exported_stuff));
}

const sema::sema_node& global_executor::get_sema_tree(
  cmsl::string_view import_path) const
{
  const auto path = build_full_import_path(import_path);
  return m_sema_trees.at(path);
}

std::string global_executor::build_full_import_path(
  cmsl::string_view import_path) const
{
  return m_root_path + '/' + std::string{ import_path };
}
}
