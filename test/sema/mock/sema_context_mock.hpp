#pragma once

#include "sema/sema_context.hpp"

#include <gmock/gmock.h>

namespace cmsl::sema::test {
class sema_context_mock : public sema_context
{
public:
  MOCK_METHOD1(add_function, void(const sema_function&));
  MOCK_METHOD1(add_type, void(const sema_type&));
  MOCK_CONST_METHOD1(find_type,
                     const sema_type*(const ast::type_representation&));
  MOCK_CONST_METHOD1(find_referenced_type,
                     const sema_type*(const ast::type_representation&));
  MOCK_CONST_METHOD1(find_reference_for, const sema_type*(const sema_type&));
  MOCK_CONST_METHOD1(find_type_in_this_scope,
                     const sema_type*(const ast::type_representation&));
  MOCK_CONST_METHOD1(find_function,
                     function_lookup_result_t(const lexer::token&));
  MOCK_CONST_METHOD1(
    find_function_in_this_scope,
    single_scope_function_lookup_result_t(const lexer::token&));
  MOCK_CONST_METHOD0(type, context_type());
  MOCK_CONST_METHOD0(types,
                     std::vector<std::reference_wrapper<const sema_type>>());
};
}
