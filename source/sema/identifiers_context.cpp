#include "sema/identifiers_context.hpp"
#include "ast/name_with_coloncolon.hpp"

namespace cmsl::sema {

identifiers_context_impl::identifiers_context_impl()
{
  (void)create_context("");
  m_current_context_path.push_back({ "", 0u });
}

void identifiers_context_impl::register_identifier(
  identifiers_context_impl::token_t declaration_token, identifier_info info)
{
  auto& ctx = current_context();
  current_ids().emplace(declaration_token, info);
}

const identifiers_context_impl::identifier_info*
identifiers_context_impl::info_of(const qualified_names_t& names) const
{
  const auto found = find(names);
  if (!found) {
    return nullptr;
  }

  const auto& found_pair = *found;
  return &(found_pair->second);
}

std::optional<identifiers_context_impl::token_t>
identifiers_context_impl::declaration_token_of(
  const qualified_names_t& names) const
{
  const auto found = find(names);
  if (!found) {
    return {};
  }

  const auto& found_pair = *found;
  return found_pair->first;
}

void identifiers_context_impl::enter_local_ctx()
{
  m_local_contextes.emplace_back(id_map_t{});
}

void identifiers_context_impl::enter_global_ctx(cmsl::string_view name)
{
  context_id_t ctx_id;

  auto current_ctx = &current_context();

  if (contains(current_ctx->ctxs, name)) {
    ctx_id = current_ctx->ctxs[name];
  } else {
    ctx_id = create_context(name);
    current_ctx = &current_context();
    current_ctx->ctxs[name] = ctx_id;
  }

  m_current_context_path.push_back({ name, ctx_id });
}

void identifiers_context_impl::leave_ctx()
{
  const auto is_in_local_context = !m_local_contextes.empty();
  if (is_in_local_context) {
    m_local_contextes.pop_back();
  } else {
    m_current_context_path.pop_back();
  }
}

std::optional<identifiers_context_impl::id_map_t::const_iterator>
identifiers_context_impl::find_in_ctx(cmsl::string_view name,
                                      const id_map_t& ids) const
{
  const auto pred = [name](const auto& id_pair) {
    return id_pair.first.str() == name;
  };

  const auto found = std::find_if(std::cbegin(ids), std::cend(ids), pred);
  if (found != std::cend(ids)) {
    return found;
  }

  return std::nullopt;
}

std::optional<identifiers_context_impl::id_map_t::const_iterator>
identifiers_context_impl::find(cmsl::string_view name) const
{
  for (auto ctx_it = std::crbegin(m_local_contextes);
       ctx_it != std::crend(m_local_contextes); ++ctx_it) {
    const auto found = find_in_ctx(name, *ctx_it);
    if (found) {
      return *found;
    }
  }

  auto* current_ctx = &current_context();

  while (true) {
    const auto found = find_in_ctx(name, current_ctx->ids);
    if (found) {
      return *found;
    }

    const auto is_root_ctx = (current_ctx->id == 0u);
    if (is_root_ctx) {
      return std::nullopt;
    }

    current_ctx = &m_contextes_container[current_ctx->parent_id];
  }
}

std::optional<identifiers_context_impl::id_map_t::const_iterator>
identifiers_context_impl::find_qualified(const qualified_names_t& names) const
{
  const auto first_name = names.front().name.str();
  auto* ctx = &current_context();

  while (true) {
    if (contains(ctx->ctxs, first_name)) {
      break;
    }

    const auto is_root_ctx = (ctx->id == 0u);
    if (is_root_ctx) {
      ctx = nullptr;
      break;
    }

    ctx = &m_contextes_container[ctx->parent_id];
  }

  if (!ctx) {
    return std::nullopt;
  }

  for (auto i = 0u; i < names.size() - 1u; ++i) {
    const auto name = names[i].name.str();
    if (!contains(ctx->ctxs, name)) {
      return std::nullopt;
    }

    const auto next_ctx_id = ctx->ctxs.at(name);
    ctx = &m_contextes_container[next_ctx_id];
  }

  const auto looked_identifier_name = names.back().name;
  const auto looked_id_name_str = looked_identifier_name.str();

  for (const auto& pair : ctx->ids) {
    auto f = pair.first.str();
    auto s = pair.second.index;
  }

  const auto found = ctx->ids.find(looked_identifier_name);
  if (found == std::cend(ctx->ids)) {
    const auto got_str = ctx->ids.cbegin()->first.str();
    return std::nullopt;
  }

  return found;
}

std::optional<identifiers_context_impl::id_map_t::const_iterator>
identifiers_context_impl::find(const qualified_names_t& names) const
{
  if (is_qualified_name(names)) {
    return find_qualified(names);
  } else {
    const auto name = names.front().name.str();
    return find(name);
  }
}

identifiers_context_impl::context_id_t
identifiers_context_impl::current_context_id() const
{
  return m_current_context_path.empty() ? k_bad_id
                                        : m_current_context_path.back().id;
}

identifiers_context_impl::context_id_t
identifiers_context_impl::create_context(cmsl::string_view name)
{
  const context_id_t id = m_contextes_container.size();
  contextes_tree_node node{ name, id, current_context_id() };
  m_contextes_container.emplace_back(node);
  auto& current_ctx = current_context();
  current_ctx.ctxs[name] = id;
  return id;
}

identifiers_context_impl::contextes_tree_node&
identifiers_context_impl::current_context()
{
  if (const auto curr_id = current_context_id(); curr_id != k_bad_id) {
    return m_contextes_container.at(curr_id);
  } else {
    return m_contextes_container[0];
  }

  const auto current_ctx_id = m_current_context_path.back().id;

  contextes_tree_node* node = &m_contextes_container[0];

  for (const auto& ctx : m_current_context_path) {
    const auto next_ctx_id = node->ctxs.at(ctx.name);
    node = &m_contextes_container[next_ctx_id];
  }

  return *node;
}

const identifiers_context_impl::contextes_tree_node&
identifiers_context_impl::current_context() const
{
  const auto* node = &m_contextes_container[0];

  for (const auto& ctx : m_current_context_path) {
    const auto next_ctx_id = node->ctxs.at(ctx.name);
    node = &m_contextes_container[next_ctx_id];
  }

  return *node;
}

identifiers_context_impl::id_map_t& identifiers_context_impl::current_ids()
{
  if (!m_local_contextes.empty()) {
    return m_local_contextes.back();
  }

  return current_context().ids;
}

bool identifiers_context_impl::is_qualified_name(
  const identifiers_context::qualified_names_t& names) const
{
  return names.size() > 1u;
}
}
