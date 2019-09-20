#include "exec/extern_argument_parser.hpp"

namespace cmsl::exec {

std::optional<bool> extern_argument_parser::try_bool(
  const std::string& value) const
{
  if (value == "true") {
    return true;
  }
  if (value == "false") {
    return false;
  }

  return std::nullopt;
}

std::optional<int_t> extern_argument_parser::try_int(
  const std::string& value) const
{
  try {
    const auto val = std::stoll(value);
    return static_cast<int_t>(val);
  } catch (const std::exception& e) {
    return std::nullopt;
  }
}

std::optional<double> extern_argument_parser::try_double(
  const std::string& value) const
{
  try {
    return std::stod(value);
  } catch (const std::exception& e) {
    return std::nullopt;
  }
}

std::optional<std::string> extern_argument_parser::try_string(
  const std::string& value) const
{
  return value;
}
}
