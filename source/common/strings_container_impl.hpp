#pragma once

#include "common/strings_container.hpp"

#include <memory>
#include <string>
#include <vector>

namespace cmsl {
class strings_container_impl : public strings_container
{
public:
  cmsl::string_view store(std::string str) override;

private:
  std::vector<std::unique_ptr<std::string>> m_strings;
};
}
