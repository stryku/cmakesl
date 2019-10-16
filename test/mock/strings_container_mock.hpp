#pragma once

#include "common/strings_container.hpp"

#include <gmock/gmock.h>

namespace cmsl::test {
class strings_container_mock : public strings_container
{
public:
  MOCK_METHOD1(store, cmsl::string_view(std::string));
};
}
