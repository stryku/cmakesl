#pragma once

#include "sema/function_lookup_result.hpp"
#include "sema/sema_type.hpp"

#include <functional>
#include <memory>
#include <vector>

namespace cmsl {
namespace ast {
class type_name_reference;
enum class type_kind;
}

namespace lexer {
class token;
}

namespace sema {
class sema_function;
class sema_type;

class sema_context
{
public:
  enum class context_type
  {
    namespace_,
    class_,
    enum_
  };

  virtual ~sema_context() = default;

  // Todo: add_* probably should be moved to a builder or so.
  virtual void add_function(const sema_function& function) = 0;
  virtual void add_type(const sema_type& type) = 0;
  virtual const sema_type* find_type(
    const ast::type_representation& name) const = 0;
  virtual const sema_type* find_referenced_type(
    const ast::type_representation& name) const = 0;
  virtual const sema_type* find_reference_for(const sema_type& type) const = 0;
  virtual const sema_type* find_type_in_this_scope(
    const ast::type_representation& name) const = 0;
  virtual function_lookup_result_t find_function(
    const lexer::token& name) const = 0;
  virtual single_scope_function_lookup_result_t find_function_in_this_scope(
    const lexer::token& name) const = 0;
  virtual context_type type() const = 0;
  virtual std::vector<std::reference_wrapper<const sema_type>> types()
    const = 0;
  virtual std::string fully_qualified_name() const = 0;

  // Returns a parent context, which contains context of the searched type.
  virtual const sema_context*
  find_ctx_for_type_lookup( // Todo: change name to something like
                            // find_ctx_containing_type
    const ast::type_representation& name) const = 0;
  virtual const sema_context* find_ctx_containing_type(
    const std::string& name) const = 0;
  virtual const sema_context* parent() const = 0;
};
}
}
