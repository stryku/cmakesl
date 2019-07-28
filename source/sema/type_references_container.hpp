#pragma once

#include <functional>
#include <string>
#include <unordered_map>

namespace cmsl::sema {
class sema_type;

// Todo: to remove
class type_references_container
{
public:
  void add_type(const sema_type& ty);
  const sema_type* find_type(const std::string& fully_qualified_name) const;

private:
  std::unordered_map<std::string, std::reference_wrapper<const sema_type>>
    m_references;
};
}
