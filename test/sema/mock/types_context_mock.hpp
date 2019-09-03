#pragma once

#include "sema/types_context.hpp"

#include <gmock/gmock.h>

namespace cmsl::sema::test {
class types_context_mock : public types_context
{
public:
  MOCK_METHOD2(register_type, void(const lexer::token&, const sema_type&));

  MOCK_CONST_METHOD1(
    find, const sema_type*(const std::vector<ast::name_with_coloncolon>&));

  MOCK_CONST_METHOD1(find_in_current_scope,
                     const sema_type*(const lexer::token& name));

  MOCK_METHOD1(enter_global_ctx, void(const lexer::token& name));
  MOCK_METHOD0(leave_ctx, void());
  MOCK_CONST_METHOD0(clone, std::unique_ptr<types_context>());
};

}