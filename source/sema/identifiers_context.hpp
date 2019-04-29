#pragma once

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

public:
  virtual ~identifiers_context() = default;

  virtual void register_identifier(token_t name, const sema_type& ty) = 0;
  virtual const sema_type* type_of(cmsl::string_view name) const = 0;
  virtual void enter_ctx() = 0;
  virtual void leave_ctx() = 0;
};

class identifiers_context_impl : public identifiers_context
{
private:
  using token_t = lexer::token;
  using id_map_t =
    std::unordered_map<token_t, std::reference_wrapper<const sema_type>>;

public:
  void register_identifier(token_t declaration_token,
                           const sema_type& ty) override
  {
    if (m_contextes.empty()) {
      CMSL_UNREACHABLE("Tried to register identifier without context");
    }

    auto& ctx = m_contextes.back();
    ctx.emplace(declaration_token, ty);
  }

  const sema_type* type_of(cmsl::string_view name) const override
  {
    const auto found = find(name);
    if (!found) {
      return nullptr;
    }

    const auto& found_pair = *found;
    return &(found_pair->second.get());
  }

  std::optional<token_t> declaration_token_of(cmsl::string_view name) const
  {
    const auto found = find(name);
    if (!found) {
      return {};
    }

    const auto& found_pair = *found;
    return found_pair->first;
  }

  void enter_ctx() override { m_contextes.emplace_back(id_map_t{}); }

  void leave_ctx() override { m_contextes.pop_back(); }

private:
  std::optional<id_map_t::const_iterator> find(cmsl::string_view name) const
  {
    const auto pred = [name](const auto& id_pair) {
      return id_pair.first.str() == name;
    };

    // Iterate contextes 'from top to down'.
    for (auto ctx_it = std::crbegin(m_contextes);
         ctx_it != std::crend(m_contextes); ++ctx_it) {
      const auto& ids = *ctx_it;
      const auto found = std::find_if(std::cbegin(ids), std::cend(ids), pred);

      if (found != std::cend(ids)) {
        return found;
      }
    }

    return {};
  }

private:
  std::vector<id_map_t> m_contextes;
};
}
