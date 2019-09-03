#pragma once

#include "sema/identifiers_context.hpp"

#include <gmock/gmock.h>

namespace cmsl::sema::test {
class identifiers_context_mock : public identifiers_context
{
public:
  MOCK_METHOD2(register_identifier, void(lexer::token, identifier_info ty));
  MOCK_CONST_METHOD1(
    info_of, std::optional<identifier_info>(const qualified_names_t& names));
  MOCK_METHOD1(enter_global_ctx, void(token_t));
  MOCK_METHOD0(enter_local_ctx, void());
  MOCK_METHOD0(leave_ctx, void());

  MOCK_CONST_METHOD0(clone, std::unique_ptr<identifiers_context>());
};
}
