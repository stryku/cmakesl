#include "sema/factories_provider.hpp"

#include "sema/factories.hpp"
#include "sema/sema_context.hpp"
#include "sema/sema_function.hpp"
#include "sema/sema_type.hpp"

namespace cmsl::sema {
factories_provider::~factories_provider()
{
}

sema_context_factory factories_provider::context_factory()
{
  return sema_context_factory{ m_contextes };
}

sema_function_factory factories_provider::function_factory()
{
  return sema_function_factory{ m_functions };
}

sema_type_factory factories_provider::type_factory(types_context& ctx)
{
  return sema_type_factory{ ctx, m_types };
}
}
