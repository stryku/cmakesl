#pragma once

#include "exec/instance/instance_value_observer.hpp"

#include <functional>

namespace cmsl::exec::inst {
class instance_value_variant;

class instance_value_accessor
{
public:
  explicit instance_value_accessor(const sema::sema_type& type,
                                   instance_value_variant& value_ref,
                                   instance_value_observer_t observer = {});
  ~instance_value_accessor();

  template <typename T>
  void set(T&& new_value)
  {
    set_value(std::forward<T>(new_value));
    call_observer();
  }

  instance_value_variant& access();

private:
  void set_value(instance_value_variant new_value);

  void call_observer() const;

private:
  const sema::sema_type& m_type;
  instance_value_variant& m_value_ref;
  instance_value_observer_t m_observer;
  bool m_possibly_dirty{ false };
};
}