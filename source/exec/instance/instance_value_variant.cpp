#include "exec/instance/instance_value_variant.hpp"
#include "common/assert.hpp"
#include "exec/instance/instance.hpp"
#include "instance_value_variant.hpp"

namespace cmsl::exec::inst {
instance_value_variant::instance_value_variant()
  : instance_value_variant{ false }
{
}

instance_value_variant::instance_value_variant(bool val)
  : m_value{ val }
{
}

instance_value_variant::instance_value_variant(int_t val)
  : m_value{ val }
{
}

instance_value_variant::instance_value_variant(double val)
  : m_value{ val }
{
}

instance_value_variant::instance_value_variant(enum_constant_value val)
  : m_value{ val }
{
}

// Prevent conversion from const char* to bool.
instance_value_variant::instance_value_variant(const char* value)
  : instance_value_variant{ std::string{ value } }
{
}

instance_value_variant::instance_value_variant(std::string val)
  : m_value{ val }
{
}

instance_value_variant::instance_value_variant(version_value val)
  : m_value{ val }
{
}

instance_value_variant::instance_value_variant(extern_value val)
  : m_value{ std::move(val) }
{
}

instance_value_variant::instance_value_variant(list_value val)
  : m_value{ val }
{
}

instance_value_variant::instance_value_variant(project_value val)
  : m_value{ val }
{
}

instance_value_variant::instance_value_variant(library_value val)
  : m_value{ val }
{
}

instance_value_variant::instance_value_variant(executable_value val)
  : m_value{ val }
{
}

instance_value_variant::instance_value_variant(option_value val)
  : m_value{ std::move(val) }
{
}

instance_value_variant::which_t instance_value_variant::which() const
{
  return static_cast<which_t>(m_value.index());
}

bool instance_value_variant::get_bool() const
{
  return std::get<bool>(m_value);
}

void instance_value_variant::set_bool(bool value)
{
  m_value = value;
}

int_t instance_value_variant::get_int() const
{
  return std::get<int_t>(m_value);
}

void instance_value_variant::set_int(int_t value)
{
  m_value = value;
}

double instance_value_variant::get_double() const
{
  return std::get<double>(m_value);
}

void instance_value_variant::set_double(double value)
{
  m_value = value;
}

enum_constant_value instance_value_variant::get_enum_constant() const
{
  return std::get<enum_constant_value>(m_value);
}

void instance_value_variant::set_enum_constant(enum_constant_value value)
{
  m_value = value;
}

const std::string& instance_value_variant::get_string_cref() const
{
  return std::get<std::string>(m_value);
}

std::string& instance_value_variant::get_string_ref()
{
  return std::get<std::string>(m_value);
}

void instance_value_variant::set_string(std::string value)
{
  m_value = std::move(value);
}

#define BINARY_OPERATOR(op)                                                   \
  if (which() != rhs.which()) {                                               \
    return false;                                                             \
  }                                                                           \
                                                                              \
  switch (which()) {                                                          \
    case which_t::bool_:                                                      \
      return get_bool() op rhs.get_bool();                                    \
    case which_t::int_:                                                       \
      return get_int() op rhs.get_int();                                      \
    case which_t::double_:                                                    \
      return get_double() op rhs.get_double();                                \
    case which_t::enum_:                                                      \
      return get_enum_constant().value op rhs.get_enum_constant().value;      \
    case which_t::string:                                                     \
      return get_string_cref() op rhs.get_string_cref();                      \
    case which_t::version:                                                    \
      return get_version_cref() op rhs.get_version_cref();                    \
    case which_t::list:                                                       \
      return get_list_cref() op rhs.get_list_cref();                          \
    case which_t::project:                                                    \
      break;                                                                  \
    case which_t::library:                                                    \
      break;                                                                  \
    case which_t::executable:                                                 \
      break;                                                                  \
    case which_t::option:                                                     \
      break;                                                                  \
    case which_t::extern_:                                                    \
      break;                                                                  \
  }                                                                           \
  CMSL_UNREACHABLE("Unknown alternative")

bool instance_value_variant::operator==(
  const instance_value_variant& rhs) const
{
  BINARY_OPERATOR(==);
}

bool instance_value_variant::operator!=(
  const instance_value_variant& rhs) const
{
  return !(*this == rhs);
}

bool instance_value_variant::operator<(const instance_value_variant& rhs) const
{
  BINARY_OPERATOR(<);
}

bool instance_value_variant::operator<=(
  const instance_value_variant& rhs) const
{
  return *this == rhs || *this < rhs;
}

bool instance_value_variant::operator>(const instance_value_variant& rhs) const
{
  return !(*this <= rhs);
}

bool instance_value_variant::operator>=(
  const instance_value_variant& rhs) const
{
  return *this == rhs || *this > rhs;
}

const version_value& instance_value_variant::get_version_cref() const
{
  return std::get<version_value>(m_value);
}

version_value& instance_value_variant::get_version_ref()
{
  return std::get<version_value>(m_value);
}

void instance_value_variant::set_version(version_value value)
{
  m_value = std::move(value);
}

const list_value& instance_value_variant::get_list_cref() const
{
  return std::get<list_value>(m_value);
}

list_value& instance_value_variant::get_list_ref()
{
  return std::get<list_value>(m_value);
}

void instance_value_variant::set_list(list_value value)
{
  m_value = std::move(value);
}

const project_value& instance_value_variant::get_project_cref() const
{
  return std::get<project_value>(m_value);
}

project_value& instance_value_variant::get_project_ref()
{
  return std::get<project_value>(m_value);
}

void instance_value_variant::set_project(project_value value)
{
  m_value = std::move(value);
}

const library_value& instance_value_variant::get_library_cref() const
{
  return std::get<library_value>(m_value);
}

library_value& instance_value_variant::get_library_ref()
{
  return std::get<library_value>(m_value);
}

void instance_value_variant::set_library(library_value value)
{
  m_value = std::move(value);
}

const executable_value& instance_value_variant::get_executable_cref() const
{
  return std::get<executable_value>(m_value);
}

executable_value& instance_value_variant::get_executable_ref()
{
  return std::get<executable_value>(m_value);
}

void instance_value_variant::set_executable(executable_value value)
{
  m_value = std::move(value);
}

const option_value& instance_value_variant::get_option_cref() const
{
  return std::get<option_value>(m_value);
}

option_value& instance_value_variant::get_option_ref()
{
  return std::get<option_value>(m_value);
}

void instance_value_variant::set_option(option_value value)
{
  m_value = value;
}

const extern_value& instance_value_variant::get_extern_cref() const
{
  return std::get<extern_value>(m_value);
}

extern_value& instance_value_variant::get_extern_ref()
{
  return std::get<extern_value>(m_value);
}

void instance_value_variant::set_extern(extern_value value)
{
  m_value.emplace<extern_value>(std::move(value));
}
}
