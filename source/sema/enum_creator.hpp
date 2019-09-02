#pragma once

#include "sema/sema_type.hpp"

#include <vector>

namespace cmsl {

namespace lexer {
class token;
}

namespace sema {
class sema_type_factory;
class sema_function_factory;
class sema_context_factory;
class sema_context;
class builtin_types_accessor;

class enum_creator
{
public:
  explicit enum_creator(sema_type_factory& type_factory,
                        sema_function_factory& function_factory,
                        sema_context_factory& context_factory,
                        sema_context& parent_ctx,
                        const builtin_types_accessor& builtin_types);

  const sema_type& create(const lexer::token& name,
                          const std::vector<lexer::token>& enumerators,
                          sema_type::flags_t additional_flags = {});

private:
  sema_type_factory& m_type_factory;
  sema_function_factory& m_function_factory;
  sema_context_factory& m_context_factory;
  sema_context& m_parent_ctx;
  const builtin_types_accessor& m_builtin_types;
};

}
}
