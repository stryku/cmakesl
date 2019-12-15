#pragma once

#include "sema/add_declarative_file_semantic_handler.hpp"

#include <gmock/gmock.h>

namespace cmsl::sema::test {
class add_declarative_file_semantic_handler_mock
  : public add_declarative_file_semantic_handler
{
public:
  MOCK_METHOD1(handle_add_declarative_file,
               add_declarative_file_result_t(cmsl::string_view));
};
}
