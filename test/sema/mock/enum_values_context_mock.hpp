#pragma once

#include "sema/enum_values_context.hpp"

#include <gmock/gmock.h>

namespace cmsl::sema::test {
class enum_values_context_mock : public enum_values_context
{
public:
  MOCK_METHOD3(register_identifier, void(lexer::token, enum_value_info, bool));
  MOCK_CONST_METHOD1(info_of,
                     std::optional<enum_value_info>(const qualified_names_t&));
  MOCK_METHOD2(enter_global_ctx, void(token_t, bool));
  MOCK_METHOD0(leave_ctx, void());
  MOCK_CONST_METHOD0(clone, std::unique_ptr<enum_values_context>());
  MOCK_CONST_METHOD0(collect_exported_stuff,
                     std::unique_ptr<enum_values_context>());
  MOCK_METHOD2(merge_imported_stuff,
               bool(const enum_values_context& imported,
                    errors::errors_observer& errs));

  MOCK_CONST_METHOD1(dump, void(qualified_contexts_dumper&));
};
}
