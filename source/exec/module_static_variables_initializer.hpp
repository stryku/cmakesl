#pragma once

#include "common/string.hpp"

namespace cmsl::exec {
class module_static_variables_initializer
{
public:
  virtual ~module_static_variables_initializer() = default;

  virtual void initialize(cmsl::string_view import_path) = 0;
};
}