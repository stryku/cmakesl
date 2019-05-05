#pragma once

#include "common/int_alias.hpp"
#include "exec/instance/instance_value_alternative.hpp"
#include "exec/instance/list_value.hpp"
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
    std::variant<bool, int_t, double, std::string, version_value, list_value,
                 project_value, library_value, executable_value>;

public:
  using which_t = instance_value_alternative;

  explicit instance_value_variant();

  instance_value_variant(bool val);

  instance_value_variant(int_t val);

  instance_value_variant(double val);

  // Prevent conversion from const char* to bool.
  instance_value_variant(const char* value);
  instance_value_variant(std::string val);

  instance_value_variant(version_value val);

  instance_value_variant(list_value val);
  instance_value_variant(project_value val);
  instance_value_variant(library_value val);
  instance_value_variant(executable_value val);

  which_t which() const;

  bool get_bool() const;
  void set_bool(bool value);

  int_t get_int() const;
  void set_int(int_t value);

  double get_double() const;
  void set_double(double value);

  const std::string& get_string_cref() const;
  std::string& get_string_ref();
  void set_string(std::string value);

  const version_value& get_version_cref() const;
  version_value& get_version_ref();
  void set_version(version_value value);

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
