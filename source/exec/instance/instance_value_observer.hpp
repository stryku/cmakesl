#pragma once

#include <functional>

namespace cmsl {
namespace sema {
class sema_type;
}

namespace exec::inst {
class instance_value_variant;
using instance_value_observer_t = std::function<void(
  const sema::sema_type& type, const instance_value_variant&)>;
}
}
