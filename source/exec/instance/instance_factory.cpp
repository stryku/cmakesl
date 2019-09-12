#include "instance_factory.hpp"

#include "common/assert.hpp"
#include "exec/instance/complex_unnamed_instance.hpp"
#include "exec/instance/instance_reference.hpp"
#include "exec/instance/simple_unnamed_instance.hpp"
#include "sema/builtin_types_accessor.hpp"
#include "sema/builtin_types_finder.hpp"
#include "sema/cmake_namespace_types_accessor.hpp"
#include "sema/sema_context.hpp"

namespace cmsl::exec::inst {
std::unique_ptr<instance> instance_factory2::create(
  instance_value_variant value,
  const sema::builtin_types_accessor& builtin_types) const
{
  const auto type_getter = [&value,
                            &builtin_types]() -> const sema::sema_type& {
    switch (value.which()) {
      // Todo: cache types, don't find it over every time.
      case instance_value_variant::which_t::bool_: {
        return builtin_types.bool_;
      }
      case instance_value_variant::which_t::int_: {
        return builtin_types.int_;
      }
      case instance_value_variant::which_t::double_: {
        return builtin_types.double_;
      }
      case instance_value_variant::which_t::string: {
        return builtin_types.string;
      }
      case instance_value_variant::which_t::library: {
        return builtin_types.cmake->library;
      }
      case instance_value_variant::which_t::executable: {
        return builtin_types.cmake->executable;
      }
      case instance_value_variant::which_t::option: {
        return builtin_types.cmake->option;
      }
      default:
        CMSL_UNREACHABLE("Unknown type requested");
    }
  };

  const auto& type = type_getter();
  return std::make_unique<simple_unnamed_instance>(type, value);
}

std::unique_ptr<instance> instance_factory2::create_reference(
  instance& referenced_instance) const
{
  return std::make_unique<instance_reference>(referenced_instance);
}

std::unique_ptr<instance> instance_factory2::create(
  const sema::sema_type& type) const
{
  if (type.is_complex() || !type.is_builtin()) {
    return std::make_unique<complex_unnamed_instance>(type);
  } else {
    return std::make_unique<simple_unnamed_instance>(type);
  }
}

std::unique_ptr<instance> instance_factory2::create(
  const sema::sema_type& type, instance_value_variant value) const
{
  return std::make_unique<simple_unnamed_instance>(type, std::move(value));
}
}
