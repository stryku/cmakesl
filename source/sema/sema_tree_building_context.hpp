#pragma once

#include "errors/errors_observer.hpp"
#include "sema/factories.hpp"

namespace cmsl::sema {
struct sema_tree_building_context
{
  errors::errors_observer errors_observer;
  sema::sema_type_factory type_factory;
  sema::sema_function_factory function_factory;
  sema::sema_context_factory context_factory;
};
}
