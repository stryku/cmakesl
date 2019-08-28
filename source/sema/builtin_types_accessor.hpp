#pragma once

namespace cmsl::sema {
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
