#include "sema/type_references_container.hpp"

#include "sema/sema_type.hpp"

namespace cmsl::sema {

void type_references_container::add_type(const sema_type& ty)
{
  const auto name = ty.fully_qualified_name();
  m_references.emplace(name, ty);
}

const sema_type* type_references_container::find_type(
  const std::string& fully_qualified_name) const
{
  const auto found = m_references.find(fully_qualified_name);
  if (found == std::cend(m_references)) {
    return nullptr;
  }

  return &found->second.get();
}
}
