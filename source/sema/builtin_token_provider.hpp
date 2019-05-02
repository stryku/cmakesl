#pragma once

#include "lexer/token.hpp"

#include <optional>
#include <unordered_map>

namespace cmsl::sema {
struct bool_tokens_provider;

// An instance of this class has to live for the whole program execution.
// String views to paths are used later by sema and execution.
class builtin_token_provider
{
public:
  explicit builtin_token_provider(
    std::optional<cmsl::string_view> builtin_documentation_path);

  bool_tokens_provider bool_() const;

private:
  enum class builtin_type
  {
    bool_
  };

  void initialize_documentation_paths();

  std::optional<cmsl::string_view> get_path(builtin_type type) const;

private:
  std::optional<cmsl::string_view> m_builtin_documentation_path;
  std::unordered_map<builtin_type, std::string> m_documentation_paths;
};
}
