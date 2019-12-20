#include "sema/builtin_token_provider.hpp"
#include "builtin_token_provider.hpp"
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

  m_documentation_paths[provider_type::bool_] = path_provider("bool.cmsl");
  m_documentation_paths[provider_type::int_] = path_provider("int.cmsl");
  m_documentation_paths[provider_type::double_] = path_provider("double.cmsl");
  m_documentation_paths[provider_type::string] = path_provider("string.cmsl");
  m_documentation_paths[provider_type::extern_] = path_provider("extern.cmsl");
  m_documentation_paths[provider_type::list] = path_provider("list.cmsl");
  m_documentation_paths[provider_type::executable] =
    path_provider("executable.cmsl");
  m_documentation_paths[provider_type::library] =
    path_provider("library.cmsl");
  m_documentation_paths[provider_type::project] =
    path_provider("project.cmsl");
  m_documentation_paths[provider_type::version] =
    path_provider("version.cmsl");
  m_documentation_paths[provider_type::void_] = path_provider("void.cmsl");
  m_documentation_paths[provider_type::option] = path_provider("option.cmsl");
  m_documentation_paths[provider_type::cmake] = path_provider("cmake.cmsl");
  m_documentation_paths[provider_type::decl] = path_provider("decl.cmsl");
}

std::optional<cmsl::string_view> builtin_token_provider::get_path(
  builtin_token_provider::provider_type type) const
{
  const auto& path = m_documentation_paths.at(type);

  if (path.empty()) {
    return std::nullopt;
  }

  return path;
}

bool_tokens_provider builtin_token_provider::bool_() const
{
  return bool_tokens_provider{ get_path(provider_type::bool_) };
}

double_tokens_provider builtin_token_provider::double_() const
{
  return double_tokens_provider{ get_path(provider_type::double_) };
}

executable_tokens_provider builtin_token_provider::executable() const
{
  return executable_tokens_provider{ get_path(provider_type::executable) };
}

library_tokens_provider builtin_token_provider::library() const
{
  return library_tokens_provider{ get_path(provider_type::library) };
}

list_tokens_provider builtin_token_provider::list() const
{
  return list_tokens_provider{ get_path(provider_type::list) };
}

project_tokens_provider builtin_token_provider::project() const
{
  return project_tokens_provider{ get_path(provider_type::project) };
}

int_tokens_provider builtin_token_provider::int_() const
{
  return int_tokens_provider{ get_path(provider_type::int_) };
}

string_tokens_provider builtin_token_provider::string() const
{
  return string_tokens_provider{ get_path(provider_type::string) };
}

version_tokens_provider builtin_token_provider::version() const
{
  return version_tokens_provider{ get_path(provider_type::version) };
}

void_tokens_provider builtin_token_provider::void_() const
{
  return void_tokens_provider{ get_path(provider_type::void_) };
}

option_tokens_provider builtin_token_provider::option() const
{
  return option_tokens_provider{ get_path(provider_type::option) };
}

cmake_tokens_provider builtin_token_provider::cmake() const
{
  return cmake_tokens_provider{ get_path(provider_type::cmake) };
}

extern_tokens_provider builtin_token_provider::extern_() const
{
  return extern_tokens_provider{ get_path(provider_type::extern_) };
}

decl_tokens_provider builtin_token_provider::decl() const
{
  return decl_tokens_provider{ get_path(provider_type::decl) };
}
}
