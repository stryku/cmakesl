#pragma once

#include "sema/sema_nodes.hpp"

#include <gmock/gmock.h>

namespace cmsl
{
    namespace sema
    {
        namespace test
        {
            class expression_node_mock : public expression_node
            {
            public:
                MOCK_CONST_METHOD0(type, const ast::type*());
            };
        }
    }
}
