#pragma once

#include "sema/enum_values_context.hpp"
#include "sema/functions_context.hpp"
#include "sema/identifiers_context.hpp"
#include "sema/qualified_contextes.hpp"
#include "sema/types_context.hpp"

namespace cmsl::sema {
struct qualified_contextes_refs
{
  explicit qualified_contextes_refs(qualified_contextes& ctxs)
    : functions{ *ctxs.functions }
    , enums{ *ctxs.enums }
    , ids{ *ctxs.ids }
    , types{ *ctxs.types }
  {
  }

  qualified_contextes_refs(enum_values_context& enums_ctx,
                           functions_context& functions_ctx,
                           identifiers_context& ids_ctx,
                           types_context& types_ctx)
    : functions{ functions_ctx }
    , enums{ enums_ctx }
    , ids{ ids_ctx }
    , types{ types_ctx }
  {
  }

  template <typename Context>
  class qualified_ctx_guard
  {
  public:
    explicit qualified_ctx_guard(Context& ids_context)
      : m_ids_ctx{ ids_context }
    {
      m_ids_ctx.enter_local_ctx();
    }
    explicit qualified_ctx_guard(Context& ids_context,
                                 const lexer::token& name, bool exported)
      : m_ids_ctx{ ids_context }
    {
      m_ids_ctx.enter_global_ctx(name, exported);
    }

    ~qualified_ctx_guard()
    {
      if (m_valid) {
        m_ids_ctx.leave_ctx();
      }
    }

    qualified_ctx_guard(qualified_ctx_guard&& other) noexcept
      : m_ids_ctx{ other.m_ids_ctx }
    {
      other.m_valid = false;
    }

    qualified_ctx_guard& operator=(qualified_ctx_guard&& other) noexcept
    {
      m_valid = true;
      other.m_valid = false;
      return *this;
    }

    qualified_ctx_guard(const qualified_ctx_guard&) = delete;
    qualified_ctx_guard& operator=(const qualified_ctx_guard&) = delete;

  private:
    Context& m_ids_ctx;
    bool m_valid{ true };
  };

  class all_qualified_contextes_guard
  {
  public:
    explicit all_qualified_contextes_guard(identifiers_context& ids,
                                           types_context& types,
                                           functions_context& functions,
                                           enum_values_context& enums,
                                           const lexer::token& name,
                                           bool exported)
      : m_ids_guard{ ids, name, exported }
      , m_types_guard{ types, name, exported }
      , m_functions_guard{ functions, name, exported }
      , m_enums_guard{ enums, name, exported }
    {
    }

    all_qualified_contextes_guard(all_qualified_contextes_guard&&) = default;
    all_qualified_contextes_guard& operator=(all_qualified_contextes_guard&&) =
      default;

    all_qualified_contextes_guard(const all_qualified_contextes_guard&) =
      delete;
    all_qualified_contextes_guard& operator=(
      const all_qualified_contextes_guard&) = delete;

  private:
    qualified_ctx_guard<identifiers_context> m_ids_guard;
    qualified_ctx_guard<types_context> m_types_guard;
    qualified_ctx_guard<functions_context> m_functions_guard;
    qualified_ctx_guard<enum_values_context> m_enums_guard;
  };

  [[nodiscard]] auto local_ids_guard() { return qualified_ctx_guard{ ids }; }
  [[nodiscard]] auto global_ids_guard(const lexer::token& name, bool exported)
  {
    return qualified_ctx_guard{ ids, name, exported };
  }

  [[nodiscard]] auto types_ctx_guard(const lexer::token& name, bool exported)
  {
    return qualified_ctx_guard{ types, name, exported };
  }

  [[nodiscard]] auto enums_ctx_guard(const lexer::token& name, bool exported)
  {
    return qualified_ctx_guard{ enums, name, exported };
  }

  [[nodiscard]] auto functions_ctx_guard(const lexer::token& name,
                                         bool exported)
  {
    return qualified_ctx_guard{ functions, name, exported };
  }

  [[nodiscard]] auto all_qualified_ctxs_guard(const lexer::token& name,
                                              bool exported)
  {
    return all_qualified_contextes_guard{ ids,   types, functions,
                                          enums, name,  exported };
  }

  functions_context& functions;
  enum_values_context& enums;
  identifiers_context& ids;
  types_context& types;
};
}
