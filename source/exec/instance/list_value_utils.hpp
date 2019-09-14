#pragma once

#include "common/string.hpp"

#include <functional>
#include <string>
#include <vector>

namespace cmsl::exec::inst {

class list_value;

class list_value_utils
{
public:
  explicit list_value_utils(const list_value& list);

  std::vector<std::string> strings(const std::string& prefix = "") const;

private:
  const list_value& m_list;
};
}
