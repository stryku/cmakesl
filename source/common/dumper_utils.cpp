#include "common/dumper_utils.hpp"

namespace cmsl {
std::string dumper_utils::m_ident;

dumper_utils::dumper_utils(std::ostream& os)
  : m_out{ os }
{
  m_ident.clear();
  out() << "-dumping begin";
}

dumper_utils::~dumper_utils()
{
  m_ident.clear();
  out() << "-dumping end";
}

dumper_utils::ident_guard::ident_guard(std::string& ident)
  : m_ident{ ident }
{
  m_ident.push_back(' ');
}

dumper_utils::ident_guard::~ident_guard()
{
  if (m_valid) {
    m_ident.pop_back();
  }
}

dumper_utils::ident_guard::ident_guard(dumper_utils::ident_guard&& other)
  : m_ident{ other.m_ident }
  , m_valid{ other.m_valid }
{
  other.m_valid = false;
}

dumper_utils::ident_guard& dumper_utils::ident_guard::operator=(
  dumper_utils::ident_guard&& other)
{
  m_valid = other.m_valid;
  other.m_valid = false;
  return *this;
}

dumper_utils::ident_guard dumper_utils::ident()
{
  return ident_guard{ m_ident };
}

dumper_utils::line_out dumper_utils::out()
{
  return line_out{ m_out };
}

void dumper_utils::push_ident()
{
  m_ident_stack.emplace(ident());
}

void dumper_utils::pop_ident()
{
  m_ident_stack.pop();
}

}
