#pragma once

#include "common/strings_container_impl.hpp"
#include "errors/errors_observer.hpp"
#include "exec/builtin_identifiers_observer.hpp"
#include "exec/cross_translation_unit_static_variables.hpp"
#include "exec/module_sema_tree_provider.hpp"
#include "sema/add_declarative_file_semantic_handler.hpp"
#include "sema/add_subdirectory_semantic_handler.hpp"
#include "sema/factories.hpp"
#include "sema/factories_provider.hpp"
#include "sema/import_handler.hpp"
#include "sema/qualified_contextes.hpp"

#include <memory>
#include <vector>

namespace cmsl {
namespace decl_sema {
class builtin_decl_namespace_context;
}

namespace facade {
class cmake_facade;
}

namespace sema {
class builtin_sema_context;
}

namespace exec {
class compiled_declarative_source;
class compiled_source;
class declarative_source_compiler;
class execution;
class source_compiler;

class global_executor
  : public sema::add_subdirectory_semantic_handler
  , public sema::add_declarative_file_semantic_handler
  , public sema::import_handler
  , public module_sema_tree_provider
{
public:
  explicit global_executor(const std::string& root_path,
                           facade::cmake_facade& cmake_facade);
  ~global_executor();

  int execute(std::string source);

  add_subdirectory_result_t handle_add_subdirectory(
    cmsl::string_view name,
    const std::vector<std::unique_ptr<sema::expression_node>>& params)
    override;

  add_declarative_file_result_t handle_add_declarative_file(
    cmsl::string_view name) override;

  std::unique_ptr<sema::qualified_contextes> handle_import(
    cmsl::string_view path) override;

  const sema::sema_node& get_sema_tree(
    cmsl::string_view import_path) const override;

private:
  void raise_no_main_function_error(const cmsl::string_view& path);
  void raise_unsuccessful_compilation_error(const cmsl::string_view& path);

  sema::qualified_contextes create_qualified_contextes() const;
  std::unique_ptr<sema::builtin_sema_context> create_builtin_context();
  std::unique_ptr<decl_sema::builtin_decl_namespace_context>
  create_decl_namespace_context();

  std::string build_full_import_path(cmsl::string_view import_path) const;

  source_compiler create_compiler(sema::qualified_contextes& ctxs);
  declarative_source_compiler create_declarative_compiler(
    sema::qualified_contextes& ctxs);

  std::optional<source_view> load_source(std::string path);
  cmsl::string_view store_source(std::string path);
  cmsl::string_view store_path(std::string path);

  bool file_exists(const std::string& path) const;

  const compiled_source* compile_file(std::string path);
  const compiled_declarative_source* compile_declarative_file(
    std::string path);
  const compiled_source* compile_source(std::string source, std::string path);

  std::unique_ptr<inst::instance> execute(const compiled_source& compiled);

  void initialize_execution_if_need();

private:
  class directory_guard
  {
  public:
    explicit directory_guard(facade::cmake_facade& cmake_facade,
                             const std::string& dir);
    ~directory_guard();

  private:
    facade::cmake_facade& m_cmake_facade;
  };

  std::string m_root_path;
  facade::cmake_facade& m_cmake_facade;
  errors::errors_observer m_errors_observer;
  strings_container_impl m_strings_container;
  // Contextes are going to be initialized with builtin stuff at builtin
  // context creation.
  sema::qualified_contextes m_builtin_qualified_contexts;
  builtin_identifiers_observer m_builtin_identifiers_observer;

  sema::factories_provider m_factories;

  std::unique_ptr<sema::builtin_token_provider> m_builtin_tokens;
  std::unique_ptr<sema::builtin_sema_context> m_builtin_context;
  std::unique_ptr<decl_sema::builtin_decl_namespace_context>
    m_decl_namespace_context;

  cross_translation_unit_static_variables m_static_variables;

  std::vector<std::string> m_sources;
  std::vector<std::string> m_paths;
  std::unordered_map<cmsl::string_view, std::unique_ptr<compiled_source>>
    m_compiled_sources;
  std::unordered_map<cmsl::string_view,
                     std::unique_ptr<compiled_declarative_source>>
    m_compiled_declarative_sources;
  std::unordered_map<cmsl::string_view,
                     std::reference_wrapper<const sema::sema_node>>
    m_sema_trees;

  std::unordered_map<cmsl::string_view, sema::qualified_contextes>
    m_exported_qualified_contextes;

  std::unique_ptr<execution> m_execution;
  std::vector<std::string> m_directories;
};
}
}
