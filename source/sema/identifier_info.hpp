#pragma once

#include <functional>

namespace cmsl::sema {
class sema_type;
struct identifier_info
{
  std::reference_wrapper<const sema_type> type;
  unsigned index;
};

struct builtin_identifier_info
{
  identifier_info id_info;
  std::string cmake_variable_name;
};
}