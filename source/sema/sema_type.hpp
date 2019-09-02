#pragma once

#include "ast/type_representation.hpp"
#include "lexer/token.hpp"
#include "sema/function_lookup_result.hpp"
#include "sema/type_member_info.hpp"

#include <optional>

#include <utility>
#include <vector>

namespace cmsl::sema {
class sema_context;
class sema_function;
class sema_type_builder;

class sema_type;

struct sema_type_reference
{
  const sema_type& referenced_type;
};

class sema_type
{
private:
  using token_t = lexer::token;

public:
  using flags_t = uint32_t;
  enum flags : flags_t
  {
    builtin = 0x1,
    designated_initializer = 0x2,
    enum_ = 0x4
  };

  explicit sema_type(const sema_type_reference& reference, flags_t f = {});
  explicit sema_type(const sema_context& ctx, ast::type_representation name,
                     std::vector<member_info> members, flags_t f = {});

  sema_type(const sema_type&) = delete;
  sema_type& operator=(sema_type&&) = delete;
  virtual ~sema_type() = default;

  const ast::type_representation& name() const;
  const sema_context& context() const;
  const std::vector<member_info>& members() const;

  std::optional<member_info> find_member(cmsl::string_view name) const;
  single_scope_function_lookup_result_t find_member_function(
    token_t name) const;

  bool is_complex() const;
  bool is_builtin() const;
  bool is_reference() const;
  bool is_designated_initializer() const;
  bool is_enum() const;
  bool operator==(const sema_type& rhs) const;
  bool operator!=(const sema_type& rhs) const;

  const sema_type& referenced_type() const;

  std::string fully_qualified_name() const;

private:
  const sema_context& m_ctx;
  ast::type_representation m_name;
  std::vector<member_info> m_members;
  const sema_type* m_referenced_type{ nullptr };
  flags_t m_flags;
};
}
