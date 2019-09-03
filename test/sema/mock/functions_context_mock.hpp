#pragma once

#include "sema/functions_context.hpp"

#include <gmock/gmock.h>

namespace cmsl::sema::test {
class functions_context_mock : public functions_context
{
public:
  MOCK_METHOD2(register_function,
               void(const lexer::token&, const sema_function&));

  MOCK_CONST_METHOD1(
    find,
    function_lookup_result_t(const std::vector<ast::name_with_coloncolon>&));

  MOCK_CONST_METHOD1(
    find_in_current_scope,
    const sema_function*(const function_signature& signature));

  MOCK_METHOD1(enter_global_ctx, void(const lexer::token& name));
  MOCK_METHOD0(leave_ctx, void());
  MOCK_CONST_METHOD0(clone, std::unique_ptr<functions_context>());
};

}