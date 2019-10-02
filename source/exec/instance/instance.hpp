#pragma once

#include "common/string.hpp"
#include "exec/instance/instance_value_accessor.hpp"
#include "exec/instance/instance_value_variant.hpp"
#include "sema/function_lookup_result.hpp"

#include <memory>

namespace cmsl {
namespace lexer {
class token;
}

namespace sema {
class sema_function;

class sema_type;
}

namespace exec::inst {
class instance
{
public:
  enum class kind
  {
    builtin,
    user
  };

  virtual ~instance() = default;

  virtual std::unique_ptr<instance> copy() const = 0;

  virtual instance_value_variant value() const = 0;
  virtual instance_value_accessor value_accessor() = 0;
  virtual const instance_value_variant& value_cref() const = 0;

  virtual void assign(instance_value_variant val) = 0;
  virtual void assign(std::unique_ptr<instance> val) = 0;
  virtual void assign_member(unsigned index,
                             std::unique_ptr<instance> val) = 0;

  virtual instance* find_member(unsigned index) = 0;
  virtual const instance* find_cmember(unsigned index) const = 0;
  virtual sema::single_scope_function_lookup_result_t find_function(
    lexer::token name) const = 0;

  virtual const sema::sema_type& type() const = 0;
};
}
}
