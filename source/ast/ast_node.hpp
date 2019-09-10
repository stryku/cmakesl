#pragma once

#include "lexer/token.hpp"

namespace cmsl {
struct source_location;

namespace ast {
class ast_node_visitor;

class ast_node
{
public:
  using token_t = lexer::token;

  virtual ~ast_node() {}

  virtual void visit(ast_node_visitor& visitor) const = 0;
  virtual source_location begin_location() const = 0;
  virtual source_location end_location() const = 0;
  virtual bool is_exported() const { return false; }

  source_range src_range() const
  {
    return source_range{ begin_location(), end_location() };
  };
};
}
}
