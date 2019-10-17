#pragma once

namespace cmsl::facade {
class add_subdirectory_facade
{
public:
  virtual ~add_subdirectory_facade() = default;

  virtual void add_subdirectory(const std::string& dir) = 0;
};
}
