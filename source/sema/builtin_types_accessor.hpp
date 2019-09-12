#pragma once

namespace cmsl::sema {
struct cmake_namespace_types_accessor;
class sema_type;

struct builtin_types_accessor
{
  const sema_type& void_;
  const sema_type& bool_;
  const sema_type& bool_ref;
  const sema_type& int_;
  const sema_type& int_ref;
  const sema_type& double_;
  const sema_type& double_ref;
  const sema_type& string;
  const sema_type& string_ref;

  const cmake_namespace_types_accessor* cmake{ nullptr };
};
}
