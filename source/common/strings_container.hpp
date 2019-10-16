#pragma once

#include "common/string.hpp"

#include <string>

namespace cmsl {
class strings_container
{
public:
  virtual ~strings_container() = default;

  virtual cmsl::string_view store(std::string str) = 0;
};
}
