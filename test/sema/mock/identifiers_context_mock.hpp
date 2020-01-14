#pragma once

#include "sema/identifiers_context.hpp"
#include "sema/qualified_contexts_dumper.hpp"

#include <gmock/gmock.h>

namespace cmsl::sema::test {
class identifiers_context_mock : public identifiers_context
{
public:
  MOCK_METHOD3(register_identifier, void(lexer::token, identifier_info, bool));
  MOCK_CONST_METHOD1(
    info_of, std::optional<identifier_info>(const qualified_names_t& names));
  MOCK_METHOD2(enter_global_ctx, void(token_t, bool));
  MOCK_METHOD0(enter_local_ctx, void());
  MOCK_METHOD0(leave_ctx, void());
  MOCK_CONST_METHOD0(is_in_global_ctx, bool());

  MOCK_CONST_METHOD0(clone, std::unique_ptr<identifiers_context>());
  MOCK_CONST_METHOD0(collect_exported_stuff,
                     std::unique_ptr<identifiers_context>());
  MOCK_METHOD2(merge_imported_stuff,
               bool(const identifiers_context& imported,
                    errors::errors_observer& errs));

  MOCK_CONST_METHOD1(dump, void(qualified_contexts_dumper&));
};
}
