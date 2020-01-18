#pragma once

#include "sema/types_context.hpp"

#include <gmock/gmock.h>

namespace cmsl::sema::test {
class types_context_mock : public types_context
{
public:
  MOCK_METHOD3(register_type,
               void(const lexer::token&, const sema_type&, bool));

  MOCK_CONST_METHOD1(find,
                     std::optional<type_with_reference>(
                       const std::vector<ast::name_with_coloncolon>&));

  MOCK_CONST_METHOD1(find_generic,
                     const sema_type*(const ast::type_representation&));

  MOCK_CONST_METHOD1(find_in_current_scope,
                     const sema_type*(const lexer::token& name));

  MOCK_METHOD2(enter_global_ctx, void(const lexer::token&, bool));
  MOCK_METHOD0(leave_ctx, void());
  MOCK_CONST_METHOD0(clone, std::unique_ptr<types_context>());
  MOCK_CONST_METHOD0(collect_exported_stuff, std::unique_ptr<types_context>());
  MOCK_METHOD2(merge_imported_stuff,
               bool(const types_context& imported,
                    errors::errors_observer& errs));

  MOCK_CONST_METHOD1(dump, void(qualified_contexts_dumper&));
};
}