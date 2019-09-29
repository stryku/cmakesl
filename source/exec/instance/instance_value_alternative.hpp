#pragma once

namespace cmsl::exec::inst {
enum class instance_value_alternative
{
  bool_,
  int_,
  double_,
  enum_,
  string,
  version,
  extern_,
  list,
  project,
  library,
  executable,
  option
};
}
