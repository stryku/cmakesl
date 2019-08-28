#pragma once

#include <string>

namespace cmsl::exec::inst {
class option_value
{
public:
  explicit option_value(const std::string& description, bool value = false);

  const std::string& description() const;

  bool value() const;

private:
  std::string m_description;
  bool m_value;
};
}
