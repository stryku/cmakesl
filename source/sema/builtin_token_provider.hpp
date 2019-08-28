#pragma once

#include "lexer/token.hpp"

#include <optional>
#include <unordered_map>

namespace cmsl::sema {
struct bool_tokens_provider;
struct double_tokens_provider;
struct executable_tokens_provider;
struct library_tokens_provider;
struct list_tokens_provider;
struct project_tokens_provider;
struct int_tokens_provider;
struct string_tokens_provider;
struct version_tokens_provider;
struct void_tokens_provider;
struct option_tokens_provider;

// An instance of this class has to live for the whole program execution.
// String views to paths are used later by sema and execution.
class builtin_token_provider
{
public:
  explicit builtin_token_provider(std::string builtin_documentation_path);

  builtin_token_provider(const builtin_token_provider&) = delete;
  builtin_token_provider& operator=(const builtin_token_provider&) = delete;
  builtin_token_provider(builtin_token_provider&&) = default;
  builtin_token_provider& operator=(builtin_token_provider&&) = default;

  bool_tokens_provider bool_() const;
  double_tokens_provider double_() const;
  executable_tokens_provider executable() const;
  library_tokens_provider library() const;
  list_tokens_provider list() const;
  project_tokens_provider project() const;
  int_tokens_provider int_() const;
  string_tokens_provider string() const;
  version_tokens_provider version() const;
  void_tokens_provider void_() const;
  option_tokens_provider option() const;

private:
  enum class builtin_type
  {
    bool_,
    double_,
    executable,
    library,
    list,
    project,
    option,
    int_,
    string,
    version,
    void_
  };

  void initialize_documentation_paths();

  std::optional<cmsl::string_view> get_path(builtin_type type) const;

private:
  std::string m_builtin_documentation_path;
  std::unordered_map<builtin_type, std::string> m_documentation_paths;
};
}
