#pragma once

namespace cmsl::sema {
class sema_type;

struct cmake_namespace_types_accessor
{
  const sema_type& cxx_compiler_id;
  const sema_type& cxx_compiler_id_ref;
  const sema_type& cxx_compiler_info;
  const sema_type& cxx_compiler_info_ref;
  const sema_type& visibility;
  const sema_type& visibility_ref;
  const sema_type& version;
  const sema_type& version_ref;
  const sema_type& library;
  const sema_type& library_ref;
  const sema_type& executable;
  const sema_type& executable_ref;
  const sema_type& project;
  const sema_type& project_ref;
  const sema_type& option;
  const sema_type& option_ref;
};
}
