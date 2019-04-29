#pragma once

#include "sema/sema_nodes.hpp"

#include <gmock/gmock.h>

namespace cmsl::sema::test {
class expression_node_mock : public expression_node
{
public:
  explicit expression_node_mock(const ast::ast_node& ast_node)
    : expression_node{ ast_node }
  {
  }

  MOCK_CONST_METHOD0(type, const sema_type&());
  MOCK_CONST_METHOD0(produces_temporary_value, bool());
  MOCK_CONST_METHOD1(visit, void(sema_node_visitor&));
  MOCK_CONST_METHOD0(begin_location, source_location());
  MOCK_CONST_METHOD0(end_location, source_location());
  MOCK_CONST_METHOD0(ast_node, const ast::ast_node&());
};
}
