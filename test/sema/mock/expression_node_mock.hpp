#pragma once

#include "sema/sema_nodes.hpp"

#include <gmock/gmock.h>

namespace cmsl::sema::test
{
            class expression_node_mock : public expression_node
            {
            public:
                MOCK_CONST_METHOD0(type, const sema_type&());
                MOCK_CONST_METHOD0(produces_temporary_value, bool());
                MOCK_CONST_METHOD1(visit, void(sema_node_visitor&));
            };
}
