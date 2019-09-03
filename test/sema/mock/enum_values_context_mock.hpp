#pragma once

#include "sema/enum_values_context.hpp"

#include <gmock/gmock.h>

namespace cmsl::sema::test {
class enum_values_context_mock : public enum_values_context
{
public:
  MOCK_METHOD2(register_identifier, void(lexer::token, enum_value_info));
  MOCK_CONST_METHOD1(info_of,
                     std::optional<enum_value_info>(const qualified_names_t&));
  MOCK_METHOD1(enter_global_ctx, void(token_t));
  MOCK_METHOD0(leave_ctx, void());
  MOCK_CONST_METHOD0(clone, std::unique_ptr<enum_values_context>());
};
}
