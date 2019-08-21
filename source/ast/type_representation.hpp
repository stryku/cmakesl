#pragma once

#include "qualified_name.hpp"

#include <optional>
#include <variant>

namespace cmsl::ast {
class type_representation
{
public:
  struct generic_type_name
  {
    generic_type_name(lexer::token_container_t ts,
                      std::vector<type_representation> nested)
      : tokens{ std::move(ts) }
      , nested_types{ std::move(nested) }
    {
    }

    std::string to_string() const
    {
      std::string result;

      for (const auto& token : tokens) {
        result += token.str();
      }

      return result;
    }

    const lexer::token& primary_name() const { return tokens[0]; }

    lexer::token_container_t tokens;
    std::vector<type_representation> nested_types;
  };

private:
  using name_t = std::variant<qualified_name, generic_type_name>;

public:
  struct is_reference_tag
  {
  };

  //  explicit type_representation(
  //    qualified_name_t qualified_name, lexer::token primary_name,
  //    lexer::token reference_token,
  //    std::vector<type_representation> nested_types = {});
  //  explicit type_representation(
  //    qualified_name_t qualified_name, lexer::token_container_t tokens,
  //    lexer::token reference_token,
  //    std::vector<type_representation> nested_types = {});
  //
  //  explicit type_representation(
  //    qualified_name_t qualified_name, lexer::token primary_name,
  //    is_reference_tag, std::vector<type_representation> nested_types = {});
  //  explicit type_representation(
  //    qualified_name_t qualified_name, lexer::token_container_t tokens,
  //    is_reference_tag, std::vector<type_representation> nested_types = {});
  //  explicit type_representation(
  //    qualified_name_t qualified_name, lexer::token primary_name,
  //    std::vector<type_representation> nested_types = {});
  //  explicit type_representation(
  //    qualified_name_t qualified_name, lexer::token_container_t tokens,
  //    std::vector<type_representation> nested_types = {});

  explicit type_representation(lexer::token name);
  type_representation(lexer::token name, is_reference_tag);

  explicit type_representation(qualified_name name);
  type_representation(qualified_name name, is_reference_tag);

  type_representation(generic_type_name name);
  type_representation(generic_type_name, is_reference_tag);

  explicit type_representation(name_t name);
  type_representation(name_t name, is_reference_tag);

  //  lexer::token primary_name() const;
  const std::vector<type_representation>& nested_types() const;
  lexer::token_container_t tokens() const;
  const qualified_name& qual_name() const;
  lexer::token primary_name_token() const;

  const name_t& name() const;
  const generic_type_name& generic_name() const;

  std::string unqualified_name() const;

  std::string to_string() const;
  std::string to_string_without_reference() const;

  bool operator==(const type_representation& rhs) const;
  bool operator!=(const type_representation& rhs) const;

  bool is_generic() const;
  bool is_reference() const;
  bool is_auto() const;

  source_range src_range() const;
  source_view source() const;

private:
  name_t m_name;

  //  qualified_name_t m_qualified_name;
  //  lexer::token_container_t m_tokens;
  std::optional<lexer::token> m_reference_token;
  bool m_is_reference{ false };

  // 'template parameters' of generic types.
  //  std::vector<type_representation> m_nested_types;
};
}

std::ostream& operator<<(std::ostream& out,
                         const cmsl::ast::type_representation&);
