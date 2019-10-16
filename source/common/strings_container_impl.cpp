#include "common/strings_container_impl.hpp"

namespace cmsl {
cmsl::string_view strings_container_impl::store(std::string str)
{
  auto str_ptr = std::make_unique<std::string>(std::move(str));
  cmsl::string_view view = *str_ptr;
  m_strings.emplace_back(std::move(str_ptr));
  return view;
}
}
