#pragma once

#include "errors/errors_observer.hpp"
#include "sema/factories_provider.hpp"
#include "sema/functions_context.hpp"
#include "sema/types_context.hpp"

namespace cmsl::sema {
struct sema_tree_building_context
{
  errors::errors_observer errors_observer;
  sema::functions_context_impl functions_ctx;
  sema::types_context_impl types_ctx;
  sema::factories_provider factories;
};
}
