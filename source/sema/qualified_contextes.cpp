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

qualified_contextes qualified_contextes::collect_exported_stuff() const
{
  return qualified_contextes{ enums->collect_exported_stuff(),
                              functions->collect_exported_stuff(),
                              ids->collect_exported_stuff(),
                              types->collect_exported_stuff() };
}

bool qualified_contextes::merge_imported_stuff(
  const qualified_contextes& imported, errors::errors_observer& errs)
{
  const auto enums_succeed =
    enums->merge_imported_stuff(*imported.enums, errs);
  const auto functions_succeed =
    functions->merge_imported_stuff(*imported.functions, errs);
  const auto ids_succeed = ids->merge_imported_stuff(*imported.ids, errs);
  const auto types_succeed =
    types->merge_imported_stuff(*imported.types, errs);

  return enums_succeed && functions_succeed && ids_succeed && types_succeed;
}
}
