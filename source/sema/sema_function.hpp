#pragma once

#include "lexer/token.hpp"
#include "sema/function_signature.hpp"

namespace cmsl::sema {
class sema_context;

class sema_function
{
public:
  virtual ~sema_function() = default;
  virtual const function_signature& signature() const = 0;
  virtual const sema_type& return_type() const = 0;
  virtual const sema_context& context() const = 0;
};
}
