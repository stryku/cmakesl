#pragma once

#include "sema/identifiers_context.hpp"

#include <gmock/gmock.h>

namespace cmsl
{
    namespace sema
    {
        namespace test
        {
            class identifiers_context_mock : public identifiers_context
            {
            public:
                MOCK_METHOD2(register_identifier, void(lexer::token::token, const ast::type* ty));
                MOCK_CONST_METHOD1(type_of, const ast::type*(cmsl::string_view));
                MOCK_METHOD0(enter_ctx, void());
                MOCK_METHOD0(leave_ctx, void());
            };
        }
    }
}
