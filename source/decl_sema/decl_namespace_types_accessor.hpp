#pragma once

namespace cmsl {
namespace sema {
class sema_type;
}

namespace decl_sema {
struct decl_namespace_types_accessor
{
  const sema::sema_type& forwarding_lists;
  const sema::sema_type& forwarding_lists_ref;
};
}
}
