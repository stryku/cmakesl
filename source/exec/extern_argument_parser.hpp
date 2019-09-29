#pragma once

#include "common/int_alias.hpp"

#include <optional>
#include <string>

namespace cmsl::exec {
class extern_argument_parser
{
public:
  std::optional<bool> try_bool(const std::string& value) const;
  std::optional<int_t> try_int(const std::string& value) const;
  std::optional<double> try_double(const std::string& value) const;
  std::optional<std::string> try_string(const std::string& value) const;
};
}
