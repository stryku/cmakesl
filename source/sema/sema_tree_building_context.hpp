#pragma once

#include "errors/errors_observer.hpp"
#include "sema/factories.hpp"
#include "sema/types_context.hpp"

namespace cmsl::sema {
struct sema_tree_building_context
{
  errors::errors_observer errors_observer;
  sema::types_context_impl types_ctx;
  sema::sema_type_factory type_factory{ types_ctx };
  sema::sema_function_factory function_factory;
  sema::sema_context_factory context_factory;
};
}
