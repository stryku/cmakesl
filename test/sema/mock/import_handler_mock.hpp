#pragma once

#include "sema/import_handler.hpp"
#include "sema/qualified_contextes.hpp"

#include <gmock/gmock.h>

namespace cmsl::sema::test {
class import_handler_mock : public import_handler
{
public:
  MOCK_METHOD1(handle_import,
               std::unique_ptr<qualified_contextes>(cmsl::string_view));
};
}