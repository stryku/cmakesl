#pragma once

#include "sema/sema_function.hpp"

namespace cmsl::sema {
class block_node;
class sema_context;

class user_sema_function : public sema_function
{
public:
  explicit user_sema_function(const sema_context& ctx,
                              const sema_type& return_type,
                              function_signature s)
    : m_ctx{ ctx }
    , m_return_type{ return_type }
    , m_signature{ std::move(s) }
  {
  }

  void set_body(const block_node& body) { m_body = &body; }

  const block_node& body() const { return *m_body; }

  const function_signature& signature() const override { return m_signature; }

  const sema_type& return_type() const override { return m_return_type; }

  const sema_context& context() const override { return m_ctx; }

private:
  // A context that this function is registered in. Can be a namespace or class
  // context.
  const sema_context& m_ctx;
  const sema_type& m_return_type;
  function_signature m_signature;
  // It will be set while building a class node. It needs to be set after
  // creation because it can refer to itself in case of a recursion.
  const block_node* m_body;
};
}