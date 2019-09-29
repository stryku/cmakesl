#include "exec/instance/simple_unnamed_instance.hpp"

#include "common/algorithm.hpp"
#include "common/assert.hpp"
#include "exec/instance/instance_value_variant.hpp"
#include "sema/sema_type.hpp"

namespace cmsl::exec::inst {
simple_unnamed_instance::simple_unnamed_instance(const sema::sema_type& type)
  : m_sema_type{ type }
  , m_data{ create_init_data() }
{
}

simple_unnamed_instance::simple_unnamed_instance(
  const sema::sema_type& type, instance_value_variant&& value)
  : m_sema_type{ type }
  , m_data{ std::move(value) }
{
}

instance_value_variant simple_unnamed_instance::create_init_data() const
{
  const auto name = m_sema_type.name().to_string();
  // Todo: find better way than comparing strings.
  if (name == "bool") {
    return false;
  } else if (name == "int") {
    return int_t{ 0 };
  } else if (name == "double") {
    return 0.0;
  } else if (name == "string") {
    return std::string{};
  } else if (name == "version") {
    return version_value{ 0u };
  } else if (starts_with(name, "list")) {
    return list_value{};
  } else if (name == "project") {
    return project_value{ "" };
  } else if (name == "option") {
    return option_value{ "" };
  } else if (m_sema_type.is_enum()) {
    return enum_constant_value{};
  } else if (starts_with(name, "extern")) {
    return extern_value{ nullptr };
  }

  CMSL_UNREACHABLE("Unknown type");
}

instance_value_variant simple_unnamed_instance::create_init_data(
  instance_value_variant val) const
{
  return val;
}

instance_value_variant simple_unnamed_instance::value() const
{
  return m_data;
}

instance_value_variant& simple_unnamed_instance::value_ref()
{
  return m_data;
}

const instance_value_variant& simple_unnamed_instance::value_cref() const
{
  return m_data;
}

void simple_unnamed_instance::assign(instance_value_variant val)
{
  m_data = val;
}

void simple_unnamed_instance::assign_member(unsigned,
                                            std::unique_ptr<instance>)
{
  CMSL_UNREACHABLE("Assinging a member of a simple instance");
}

instance* simple_unnamed_instance::find_member(unsigned)
{
  return nullptr;
}

const instance* simple_unnamed_instance::find_cmember(unsigned) const
{
  return nullptr;
}

std::unique_ptr<instance> simple_unnamed_instance::copy() const
{
  return std::make_unique<simple_unnamed_instance>(m_sema_type, value());
}

sema::single_scope_function_lookup_result_t
simple_unnamed_instance::find_function(lexer::token name) const
{
  return m_sema_type.find_member_function(name);
}

const sema::sema_type& simple_unnamed_instance::type() const
{
  return m_sema_type;
}

void simple_unnamed_instance::assign(std::unique_ptr<instance> val)
{
  // If we're here, we know that the val is an object of
  // simple_unnamed_instance type, so the static_cast is safe.
  auto casted_instance = static_cast<simple_unnamed_instance*>(val.get());
  m_data = std::move(casted_instance->m_data);
}
}
