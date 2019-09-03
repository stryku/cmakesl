#pragma once

#include <memory>

namespace cmsl::sema {
class enum_values_context;
class functions_context;
class identifiers_context;
class types_context;

struct qualified_contextes
{
  ~qualified_contextes();

  std::unique_ptr<enum_values_context> enums;
  std::unique_ptr<functions_context> functions;
  std::unique_ptr<identifiers_context> ids;
  std::unique_ptr<types_context> types;

  qualified_contextes clone() const;
};
}