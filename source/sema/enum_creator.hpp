#pragma once

#include "sema/sema_type.hpp"

#include <vector>

namespace cmsl {

namespace lexer {
class token;
}

namespace sema {
class factories_provider;
class types_context;
class sema_context;
class builtin_types_accessor;

class enum_creator
{
public:
  explicit enum_creator(factories_provider& factories,
                        types_context& types_ctx, sema_context& parent_ctx,
                        const builtin_types_accessor& builtin_types);

  const sema_type& create(const lexer::token& name,
                          const std::vector<lexer::token>& enumerators,
                          sema_type::flags_t additional_flags = {});

private:
  factories_provider& m_factories;
  types_context& m_types_ctx;
  sema_context& m_parent_ctx;
  const builtin_types_accessor& m_builtin_types;
};

}
}
