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
  const sema::sema_type& product;
  const sema::sema_type& product_ref;
  const sema::sema_type& executable;
  const sema::sema_type& executable_ref;
  const sema::sema_type& static_library;
  const sema::sema_type& static_library_ref;
  const sema::sema_type& shared_library;
  const sema::sema_type& shared_library_ref;
};
}
}
