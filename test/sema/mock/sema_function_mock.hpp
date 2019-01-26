#pragma once

#include "sema/sema_function.hpp"

#include <gmock/gmock.h>

namespace cmsl
{
    namespace sema
    {
        namespace test
        {
            class sema_function_mock : public sema_function
            {
            public:
                MOCK_CONST_METHOD0(body, const block_node&());
                MOCK_CONST_METHOD0(signature, const function_signature&());
                MOCK_CONST_METHOD0(return_type, const sema_type&());
            };
        }
    }
}
