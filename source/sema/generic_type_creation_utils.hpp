#pragma once

namespace cmsl {
namespace ast {
class type_representation;
}

namespace errors {
class errors_observer;
}

namespace sema {

class builtin_token_provider;
class builtin_types_accessor;
class sema_generic_type_factory;
class sema_context;
class factories_provider;
class sema_type;
class types_context;

class generic_type_creation_utils
{
public:
  explicit generic_type_creation_utils(
    sema_context& generic_types_context, factories_provider& factories,
    errors::errors_observer& errors_observer,
    const builtin_token_provider& builtin_token_provider,
    const builtin_types_accessor& builtin_types, types_context& types_ctx);

  const sema_type& list_of_strings();

private:
  const sema_type& get_or_create_generic_type(
    const ast::type_representation& type_representation);

private:
  sema_context& m_generic_types_context;
  factories_provider& m_factories;
  errors::errors_observer& m_errors_observer;
  const builtin_token_provider& m_builtin_token_provider;
  const builtin_types_accessor& m_builtin_types;
  types_context& m_types_ctx;
};
}
}
