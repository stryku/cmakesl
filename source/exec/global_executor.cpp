#include "exec/global_executor.hpp"
#include "exec/compiled_source.hpp"
#include "exec/execution.hpp"
#include "exec/source_compiler.hpp"
#include "sema/user_sema_function.hpp"

#include "cmake_facade.hpp"
#include "global_executor.hpp"

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
{
  m_cmake_facade.go_into_subdirectory(m_root_path);
}

global_executor::~global_executor() = default;

int global_executor::execute(std::string source)
{
  source_compiler compiler{ m_errors_observer, m_type_factory,
                            m_function_factory, m_context_factory, *this };
  const auto source_path = m_root_path + "/CMakeLists.cmsl";
  const auto src_view = source_view{ source_path, source };

  m_sources.emplace_back(std::move(source));
  auto compiled = compiler.compile(src_view);
  const auto main_function = compiled->get_main();
  if (!main_function) {
    raise_no_main_function_error(m_root_path);
    return -1;
  }

  const auto& global_context = compiled->get_global_context();

  m_compiled_sources.emplace_back(std::move(compiled));

  const auto casted =
    dynamic_cast<const sema::user_sema_function*>(main_function);

  execution e{ m_cmake_facade };

  inst::instances_holder instances{ global_context };
  auto main_result = e.call(*casted, {}, instances);
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
  m_paths.emplace_back(std::move(path));

  std::string source(std::istreambuf_iterator<char>{ file }, {});
  const auto src_view = source_view{ path_view, source };

  m_sources.emplace_back(std::move(source));

  source_compiler compiler{ m_errors_observer, m_type_factory,
                            m_function_factory, m_context_factory, *this };

  auto compiled = compiler.compile(src_view);
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
  m_errors_observer.nofify_error(err);
}
}
