#pragma once

#include "sema/identifiers_context.hpp"

#include <gmock/gmock.h>

namespace cmsl::sema::test
{
            class identifiers_context_mock : public identifiers_context
            {
            public:
                MOCK_METHOD2(register_identifier, void(lexer::token::token, const sema_type* ty));
                MOCK_CONST_METHOD1(type_of, const sema_type*(cmsl::string_view));
                MOCK_METHOD0(enter_ctx, void());
                MOCK_METHOD0(leave_ctx, void());
            };
}
