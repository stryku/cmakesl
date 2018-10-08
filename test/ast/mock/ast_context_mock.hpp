#pragma once

#include "ast/ast_context.hpp"

#include <gmock/gmock.h>

namespace cmsl
{
    namespace ast
    {
        namespace test
        {
            class ast_context_mock : public ast_context
            {
            public:
                MOCK_METHOD1(add_type, void(std::unique_ptr<type>));
                MOCK_CONST_METHOD1(find_type, const type*(cmsl::string_view));

                MOCK_METHOD1(add_function, void(std::unique_ptr<function>));
                MOCK_CONST_METHOD1(find_function, const function*(cmsl::string_view));
            };
        }
    }
}
