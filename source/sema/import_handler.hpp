#pragma once

#include "common/string.hpp"

#include <memory>

namespace cmsl::sema {
class qualified_contextes;

class import_handler
{
public:
  virtual ~import_handler() = default;

  // It could be std::optional, but gmock complains about its copy ctor.
  virtual std::unique_ptr<qualified_contextes> handle_import(
    cmsl::string_view path) = 0;
};
}
