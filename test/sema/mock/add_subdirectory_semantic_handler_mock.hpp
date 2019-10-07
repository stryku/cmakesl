#pragma once

#include "sema/add_subdirectory_handler.hpp"

#include <gmock/gmock.h>

namespace cmsl::sema::test {
class add_subdirectory_semantic_handler_mock : public add_subdirectory_handler
{
public:
  MOCK_METHOD2(handle_add_subdirectory,
               add_subdirectory_result_t(
                 cmsl::string_view,
                 const std::vector<std::unique_ptr<expression_node>>&));
};
}
