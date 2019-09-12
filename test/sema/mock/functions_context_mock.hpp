#pragma once

#include "sema/functions_context.hpp"

#include <gmock/gmock.h>

namespace cmsl::sema::test {
class functions_context_mock : public functions_context
{
public:
  MOCK_METHOD3(register_function,
               void(const lexer::token&, const sema_function&, bool));

  MOCK_CONST_METHOD1(
    find,
    function_lookup_result_t(const std::vector<ast::name_with_coloncolon>&));

  MOCK_CONST_METHOD1(
    find_in_current_scope,
    const sema_function*(const function_signature& signature));

  MOCK_METHOD2(enter_global_ctx, void(const lexer::token&, bool));
  MOCK_METHOD0(leave_ctx, void());
  MOCK_CONST_METHOD0(clone, std::unique_ptr<functions_context>());
  MOCK_CONST_METHOD0(collect_exported_stuff,
                     std::unique_ptr<functions_context>());
  MOCK_METHOD2(merge_imported_stuff,
               bool(const functions_context& imported,
                    errors::errors_observer& errs));
};
}