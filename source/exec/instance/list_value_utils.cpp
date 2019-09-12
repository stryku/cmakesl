#include "exec/instance/list_value_utils.hpp"
#include "exec/instance/instance.hpp"
#include "exec/instance/list_value.hpp"

namespace cmsl::exec::inst {

list_value_utils::list_value_utils(const list_value& list)
  : m_list{ list }
{
}
std::vector<std::string> list_value_utils::strings(
  const std::string& prefix) const
{
  std::vector<std::string> collected;
  collected.reserve(m_list.size());

  for (auto i = 0u; i < m_list.size(); ++i) {
    const auto& instance = m_list.at(i);
    const auto& source = instance.value_cref().get_string_cref();
    const auto full_source_path = prefix + source;
    collected.emplace_back(full_source_path);
  }

  return collected;
}
}
