#include "sema/function_signature.hpp"
#include "sema/sema_type.hpp"

#include <tuple>
namespace cmsl::sema {
bool parameter_declaration::operator==(const parameter_declaration& rhs) const
{
  return std::tie(ty, name) == std::tie(rhs.ty, rhs.name);
}

bool function_signature::operator==(const function_signature& rhs) const
{
  return std::tie(name, params) == std::tie(rhs.name, rhs.params);
}
}