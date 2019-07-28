#pragma once

#include "ast/name_with_coloncolon.hpp"
#include "common/algorithm.hpp"
#include "common/assert.hpp"
#include "lexer/token.hpp"

#include <algorithm>
#include <functional>

namespace cmsl::sema {
class sema_type;

// Todo: exists only for test purpose. Consider removing it and using shadow
// mocking.
class identifiers_context
{
protected:
  using token_t = lexer::token;
  using qualified_names_t = std::vector<ast::name_with_coloncolon>;

public:
  struct identifier_info
  {
    std::reference_wrapper<const sema_type> type;
    unsigned index;
  };

  virtual ~identifiers_context() = default;

  virtual void register_identifier(token_t name, identifier_info info) = 0;
  virtual const identifier_info* info_of(
    const qualified_names_t& names) const = 0;

  virtual void enter_global_ctx(cmsl::string_view name) = 0;
  virtual void enter_local_ctx() = 0;
  virtual void leave_ctx() = 0;
};

class identifiers_context_impl : public identifiers_context
{
private:
  using token_t = lexer::token;
  using id_map_t = std::unordered_map<token_t, identifier_info>;

  using context_id_t = unsigned;
  static constexpr auto k_bad_id = static_cast<context_id_t>(-1);

  struct contextes_tree_node
  {
    cmsl::string_view name;
    context_id_t id{ k_bad_id };
    context_id_t parent_id{ k_bad_id };
    id_map_t ids;
    std::unordered_map<cmsl::string_view, context_id_t> ctxs;
  };

  struct context_id_and_name
  {
    cmsl::string_view name;
    context_id_t id;
  };

public:
  explicit identifiers_context_impl();

  void register_identifier(token_t declaration_token,
                           identifier_info info) override;

  const identifier_info* info_of(
    const qualified_names_t& names) const override;
  std::optional<token_t> declaration_token_of(
    const qualified_names_t& names) const;

  void enter_global_ctx(cmsl::string_view name) override;
  void enter_local_ctx() override;
  void leave_ctx() override;

private:
  std::optional<id_map_t::const_iterator> find(
    const qualified_names_t& names) const;
  std::optional<id_map_t::const_iterator> find(cmsl::string_view name) const;
  std::optional<id_map_t::const_iterator> find_qualified(
    const qualified_names_t& names) const;
  std::optional<id_map_t::const_iterator> find_in_ctx(
    cmsl::string_view name, const id_map_t& ids) const;

  context_id_t current_context_id() const;
  context_id_t create_context(cmsl::string_view name);
  contextes_tree_node& current_context();
  const contextes_tree_node& current_context() const;
  id_map_t& current_ids();

  bool is_qualified_name(const qualified_names_t& names) const;

private:
  std::vector<contextes_tree_node> m_contextes_container;
  std::vector<context_id_and_name> m_current_context_path;
  std::vector<id_map_t> m_local_contextes;
};
}
