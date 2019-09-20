#pragma once

#include <memory>

namespace cmsl::exec::inst {
class instance;

class extern_value
{
public:
  explicit extern_value(std::unique_ptr<instance> value);

  extern_value(const extern_value& other);
  extern_value& operator=(const extern_value& other);

  ~extern_value();

  bool has_value() const;

  const instance& value_cref() const;

private:
  std::unique_ptr<instance> m_value;
};
}
