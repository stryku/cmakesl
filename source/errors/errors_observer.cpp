#include "errors/errors_observer.hpp"
#include "cmake_facade.hpp"
#include "errors/error.hpp"

#include <iostream>
#include <sstream>

namespace {
cmsl::string_view to_string(cmsl::errors::error_type type)
{
  switch (type) {
    case cmsl::errors::error_type::error:
      return "error";
    case cmsl::errors::error_type::warning:
      return "warning";
    case cmsl::errors::error_type::note:
      return "note";
  }
}
}

namespace cmsl::errors {

errors_observer::errors_observer(facade::cmake_facade* f)
  : m_facade{ f }
  , m_out{ m_facade ? nullptr : &std::cerr }
{
}

errors_observer::errors_observer(std::ostream& out)
  : m_out{ &out }
{
}

void errors_observer::notify_error(const error& error)
{
  const auto str = format_error(error);
  if (m_facade != nullptr) {
    m_facade->error(str);
  } else {
    (*m_out) << str;
  }
}

std::string errors_observer::format_error(const error& err) const
{
  std::stringstream ss;

  ss << err.source_path << ':' << err.range.begin.line << ':'
     << err.range.begin.column << ": " << to_string(err.type) << ": "
     << err.message << '\n';

  if (!err.line_snippet.empty()) {
    const auto range_start = err.range.begin.absolute - err.line_start_pos;
    std::string range_indicator(range_start, ' ');
    range_indicator += std::string(err.range.size(), '^');

    ss << err.line_snippet << '\n' << range_indicator << '\n';
  }

  return ss.str();
}
}
