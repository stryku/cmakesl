#pragma once

#include "sema/sema_type.hpp"

namespace cmsl::sema {
class enum_type : public sema_type
{
public:
  explicit enum_type(const sema_context& ctx, ast::type_representation name,
                     std::vector<lexer::token> enumerators)
    : sema_type{ enum_tag{}, ctx, std::move(name) }
    , m_enumerators{ std::move(enumerators) }
  {
  }

  const std::vector<lexer::token>& enumerators() const
  {
    return m_enumerators;
  }

  const lexer::token& enumerator(unsigned value) const
  {
    return m_enumerators[value];
  }

private:
  std::vector<lexer::token> m_enumerators;
};
}
