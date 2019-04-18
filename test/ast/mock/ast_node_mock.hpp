#pragma once

#include "ast/ast_node.hpp"
#include "ast/ast_node_visitor.hpp"

#include <gmock/gmock.h>

namespace cmsl::ast::test
{
    class ast_node_mock : public ast_node
    {
    public:
        MOCK_CONST_METHOD1(visit, void(ast_node_visitor &visitor));
        MOCK_CONST_METHOD0(begin_location, source_location());
        MOCK_CONST_METHOD0(end_location, source_location());
    };
}
