#pragma once

#include <memory>
#include <vector>

namespace cmsl::sema {
class sema_context;
class sema_function;
class sema_type;
class sema_context_factory;
class sema_function_factory;
class sema_type_factory;
class types_context;

class factories_provider
{
public:
  ~factories_provider();

  sema_context_factory context_factory();
  sema_function_factory function_factory();
  sema_type_factory type_factory(types_context& ctx);

private:
  std::vector<std::unique_ptr<sema_context>> m_contextes;
  std::vector<std::unique_ptr<sema_function>> m_functions;
  std::vector<std::unique_ptr<sema_type>> m_types;
};
}