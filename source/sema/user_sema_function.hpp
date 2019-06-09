#pragma once

#include "sema/sema_function.hpp"

namespace cmsl::sema {
class block_node;
class sema_context;

class user_sema_function : public sema_function
{
public:
  explicit user_sema_function(const sema_context& ctx,
                              const sema_type* return_type,
                              function_signature s)
    : m_ctx{ ctx }
    , m_return_type{ return_type }
    , m_signature{ std::move(s) }
  {
  }

  // Todo: consider creating user_sema_funciton manipulator class
  void set_body(const block_node& body) { m_body = &body; }
  void set_return_type(const sema_type& ty) { m_return_type = &ty; }

  const block_node& body() const { return *m_body; }
  const function_signature& signature() const override { return m_signature; }
  const sema_context& context() const override { return m_ctx; }
  const sema_type& return_type() const override { return *m_return_type; }

  // It should used only by sema_builder_ast_visitor while creating sema tree.
  const sema_type* try_return_type() const override { return m_return_type; }

private:
  // A context that this function is registered in. Can be a namespace or class
  // context.
  const sema_context& m_ctx;
  // In completely created function, return type should never be null. But,
  // when the function has `auto` as a return type, it has to be created
  // anyway, and its return will be deduced later, based on return statements,
  // and set using `set_return_type()` method.
  const sema_type* m_return_type;
  function_signature m_signature;
  // It will be set while building a class node. It needs to be set after
  // creation because it can refer to itself in case of a recursion.
  const block_node* m_body;
};
}