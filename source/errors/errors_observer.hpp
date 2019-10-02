#pragma once

#include <string>

namespace cmsl {
namespace facade {
class cmake_facade;
}

namespace errors {
struct error;

class errors_observer
{
public:
  explicit errors_observer(std::ostream& out);
  explicit errors_observer(facade::cmake_facade* facade = nullptr);

  void notify_error(const error& error);

private:
  std::string format_error(const error& err) const;

private:
  facade::cmake_facade* m_facade{ nullptr };
  std::ostream* m_out{ nullptr };
};
}
}
