#pragma once

#include "sema/sema_function.hpp"

namespace cmsl {
namespace sema {
enum class builtin_function_kind;
class sema_context;

class builtin_sema_function : public sema_function
{
public:
  explicit builtin_sema_function(const sema_context& ctx,
                                 const sema_type& return_type,
                                 function_signature s,
                                 builtin_function_kind kind)
    : m_ctx{ ctx }
    , m_return_type{ return_type }
    , m_signature{ std::move(s) }
    , m_kind{ kind }
  {
  }

  const function_signature& signature() const override { return m_signature; }

  const sema_type& return_type() const override { return m_return_type; }
  const sema_type* try_return_type() const override { return &m_return_type; }

  builtin_function_kind kind() const { return m_kind; }

  const sema_context& context() const override { return m_ctx; }

private:
  const sema_context& m_ctx;
  const sema_type& m_return_type;
  function_signature m_signature;
  builtin_function_kind m_kind;
};
}
}
