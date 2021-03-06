#pragma once

#include "common/int_alias.hpp"
#include "exec/instance/enum_constant_value.hpp"
#include "exec/instance/extern_value.hpp"
#include "exec/instance/instance_value_alternative.hpp"
#include "exec/instance/list_value.hpp"
#include "exec/instance/option_value.hpp"
#include "exec/instance/project_value.hpp"
#include "exec/instance/target_value.hpp"
#include "exec/instance/version_value.hpp"

#include <string>
#include <variant>
#include <vector>

namespace cmsl::exec::inst {
class instance;

class instance_value_variant
{
private:
  using value_t =
    std::variant<bool, int_t, double, enum_constant_value, std::string,
                 version_value, extern_value, list_value, project_value,
                 library_value, executable_value, option_value>;

public:
  using which_t = instance_value_alternative;

  explicit instance_value_variant();

  instance_value_variant(instance_value_variant&&) = default;
  instance_value_variant& operator=(instance_value_variant&&) = default;
  instance_value_variant(const instance_value_variant&) = default;
  instance_value_variant& operator=(const instance_value_variant&) = default;

  instance_value_variant(bool val);
  instance_value_variant(int_t val);
  instance_value_variant(double val);
  instance_value_variant(enum_constant_value val);
  // Prevent conversion from const char* to bool.
  instance_value_variant(const char* value);
  instance_value_variant(std::string val);
  instance_value_variant(version_value val);
  instance_value_variant(extern_value val);
  instance_value_variant(list_value val);
  instance_value_variant(project_value val);
  instance_value_variant(library_value val);
  instance_value_variant(executable_value val);
  instance_value_variant(option_value val);

  which_t which() const;

  bool get_bool() const;
  void set_bool(bool value);

  int_t get_int() const;
  void set_int(int_t value);

  double get_double() const;
  void set_double(double value);

  enum_constant_value get_enum_constant() const;
  void set_enum_constant(enum_constant_value value);

  const std::string& get_string_cref() const;
  std::string& get_string_ref();
  void set_string(std::string value);

  const version_value& get_version_cref() const;
  version_value& get_version_ref();
  void set_version(version_value value);

  const extern_value& get_extern_cref() const;
  extern_value& get_extern_ref();
  void set_extern(extern_value value);

  const list_value& get_list_cref() const;
  list_value& get_list_ref();
  void set_list(list_value value);

  const project_value& get_project_cref() const;
  project_value& get_project_ref();
  void set_project(project_value value);

  const library_value& get_library_cref() const;
  library_value& get_library_ref();
  void set_library(library_value value);

  const executable_value& get_executable_cref() const;
  executable_value& get_executable_ref();
  void set_executable(executable_value value);

  const option_value& get_option_cref() const;
  option_value& get_option_ref();
  void set_option(option_value value);

  // Todo: move to tests. It's not needed in production code.
  bool operator==(const instance_value_variant& rhs) const;
  bool operator!=(const instance_value_variant& rhs) const;
  bool operator<(const instance_value_variant& rhs) const;
  bool operator<=(const instance_value_variant& rhs) const;
  bool operator>(const instance_value_variant& rhs) const;
  bool operator>=(const instance_value_variant& rhs) const;

private:
  value_t m_value;
};
}
