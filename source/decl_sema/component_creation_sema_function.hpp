#pragma once

#include "sema/sema_function.hpp"

namespace cmsl::decl_sema {
class component_node;

class component_creation_sema_function : public sema::sema_function
{
public:
  explicit component_creation_sema_function(const sema::sema_context& ctx,
                                            const sema::sema_type& return_type,
                                            sema::function_signature signature,
                                            const component_node& node)
    : m_ctx{ ctx }
    , m_return_type{ return_type }
    , m_signature{ std::move(signature) }
    , m_component_node{ node }
  {
  }

  const sema::function_signature& signature() const override
  {
    return m_signature;
  }

  const sema::sema_type& return_type() const override { return m_return_type; }

  const sema::sema_type* try_return_type() const override
  {
    return &m_return_type;
  }

  const sema::sema_context& context() const override { return m_ctx; }

  const component_node& component() const { return m_component_node; }

private:
  const sema::sema_context& m_ctx;
  const sema::sema_type& m_return_type;
  sema::function_signature m_signature;
  const component_node& m_component_node;
};
}
