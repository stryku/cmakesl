#pragma once

#include "ast/ast_node.hpp"
#include "ast/parameter_declaration.hpp"
#include "ast/type_representation.hpp"

#include <memory>

namespace cmsl::ast {
class block_node;

class user_function_node : public ast_node
{
public:
  using params_t = std::vector<param_declaration>;

  user_function_node(type_representation return_type, token_t name,
                     token_t open_paren, params_t params, token_t close_paren,
                     std::unique_ptr<block_node> body);

  const type_representation& return_type_representation() const;
  token_t name() const;
  token_t open_paren() const;
  token_t close_paren() const;
  const block_node& body() const;
  const params_t& param_declarations() const;

  void visit(ast_node_visitor& visitor) const override;
  source_location begin_location() const override;
  source_location end_location() const override;

private:
  type_representation m_return_type;
  token_t m_name;
  token_t m_open_paren;
  params_t m_params;
  token_t m_close_paren;
  std::unique_ptr<block_node> m_body;
};
}
