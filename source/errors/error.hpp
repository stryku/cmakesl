#pragma once

#include "common/source_location.hpp"
#include <string>

namespace cmsl::errors {
enum class error_type
{
  error,
  warning,
  note
};

struct error
{
  cmsl::string_view source_path;
  cmsl::string_view line_snippet;
  cmsl::string_view::size_type line_start_pos;
  std::string message;
  error_type type;
  source_range range;
};
}
