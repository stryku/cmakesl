#include "sema/qualified_contextes.hpp"

#include "sema/enum_values_context.hpp"
#include "sema/functions_context.hpp"
#include "sema/identifiers_context.hpp"
#include "sema/types_context.hpp"

namespace cmsl::sema {

qualified_contextes::~qualified_contextes()
{
}

qualified_contextes qualified_contextes::clone() const
{
  return qualified_contextes{ enums->clone(), functions->clone(), ids->clone(),
                              types->clone() };
}
}
