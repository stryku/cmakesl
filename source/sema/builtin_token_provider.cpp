#include "sema/builtin_token_provider.hpp"
#include "generated/builtin_token_providers.hpp"

namespace cmsl::sema {
builtin_token_provider::builtin_token_provider(
  std::string builtin_documentation_path)
  : m_builtin_documentation_path{ std::move(builtin_documentation_path) }
{
  initialize_documentation_paths();
}

void builtin_token_provider::initialize_documentation_paths()
{
  const auto path_provider = [this](std::string file) {
    if (m_builtin_documentation_path.empty()) {
      return std::string{};
    }

    return m_builtin_documentation_path + '/' + file;
  };

  m_documentation_paths[builtin_type::bool_] = path_provider("bool.cmsl");
  m_documentation_paths[builtin_type::int_] = path_provider("int.cmsl");
  m_documentation_paths[builtin_type::double_] = path_provider("double.cmsl");
  m_documentation_paths[builtin_type::string] = path_provider("string.cmsl");
  m_documentation_paths[builtin_type::list] = path_provider("list.cmsl");
  m_documentation_paths[builtin_type::executable] =
    path_provider("executable.cmsl");
  m_documentation_paths[builtin_type::library] = path_provider("library.cmsl");
  m_documentation_paths[builtin_type::project] = path_provider("project.cmsl");
  m_documentation_paths[builtin_type::version] = path_provider("version.cmsl");
  m_documentation_paths[builtin_type::void_] = path_provider("void.cmsl");
}

std::optional<cmsl::string_view> builtin_token_provider::get_path(
  builtin_token_provider::builtin_type type) const
{
  const auto& path = m_documentation_paths.at(type);

  if (path.empty()) {
    return std::nullopt;
  }

  return path;
}

bool_tokens_provider builtin_token_provider::bool_() const
{
  return bool_tokens_provider{ get_path(builtin_type::bool_) };
}

double_tokens_provider builtin_token_provider::double_() const
{
  return double_tokens_provider{ get_path(builtin_type::double_) };
}

executable_tokens_provider builtin_token_provider::executable() const
{
  return executable_tokens_provider{ get_path(builtin_type::executable) };
}

library_tokens_provider builtin_token_provider::library() const
{
  return library_tokens_provider{ get_path(builtin_type::library) };
}

list_tokens_provider builtin_token_provider::list() const
{
  return list_tokens_provider{ get_path(builtin_type::list) };
}

project_tokens_provider builtin_token_provider::project() const
{
  return project_tokens_provider{ get_path(builtin_type::project) };
}

int_tokens_provider builtin_token_provider::int_() const
{
  return int_tokens_provider{ get_path(builtin_type::int_) };
}

string_tokens_provider builtin_token_provider::string() const
{
  return string_tokens_provider{ get_path(builtin_type::string) };
}

version_tokens_provider builtin_token_provider::version() const
{
  return version_tokens_provider{ get_path(builtin_type::version) };
}
void_tokens_provider builtin_token_provider::void_() const
{
  return void_tokens_provider{ get_path(builtin_type::void_) };
}
}
