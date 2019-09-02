#pragma once

namespace cmsl::sema {
class sema_type;

struct cmake_namespace_types_accessor
{
  const sema_type& cxx_compiler_id;
  const sema_type& cxx_compiler_id_ref;
  const sema_type& cxx_compiler_info;
  const sema_type& cxx_compiler_info_ref;
};
}
