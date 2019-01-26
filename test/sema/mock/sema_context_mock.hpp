#pragma once

#include "sema/sema_context.hpp"

#include <gmock/gmock.h>

namespace cmsl
{
    namespace sema
    {
        namespace test
        {
            class sema_context_mock : public sema_context_interface
            {
            public:
                MOCK_METHOD1(add_function, void(const sema_function&));
                MOCK_METHOD1(add_type, void(std::unique_ptr<sema_type>));
                MOCK_CONST_METHOD1(find_type, const sema_type*(cmsl::string_view));
                MOCK_CONST_METHOD1(find_type_in_this_scope, const sema_type*(cmsl::string_view));
                MOCK_CONST_METHOD1(find_function, const sema_function*(cmsl::string_view));
                MOCK_CONST_METHOD1(find_function_in_this_scope, const sema_function*(cmsl::string_view));
            };
        }
    }
}
