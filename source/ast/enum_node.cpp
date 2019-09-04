
#include "ast/enum_node.hpp"

#include "ast/ast_node_visitor.hpp"

namespace cmsl::ast {

enum_node::enum_node(std::optional<token_t> export_kw, token_t enum_kw,
                     token_t name, token_t open_brace,
                     std::vector<token_t> enumerators, token_t close_brace,
                     token_t semicolon)
  : m_export_kw{ export_kw }
  , m_enum_kw{ enum_kw }
  , m_name{ name }
  , m_open_brace{ open_brace }
  , m_enumerators{ std::move(enumerators) }
  , m_close_brace{ close_brace }
  , m_semicolon{ semicolon }
{
}

const std::optional<ast_node::token_t>& enum_node::export_() const
{
  return m_export_kw;
}

const ast_node::token_t& enum_node::enum_kw() const
{
  return m_enum_kw;
}

const ast_node::token_t& enum_node::name() const
{
  return m_name;
}

const ast_node::token_t& enum_node::open_brace() const
{
  return m_open_brace;
}

const std::vector<ast_node::token_t>& enum_node::enumerators() const
{
  return m_enumerators;
}

const ast_node::token_t& enum_node::close_brace() const
{
  return m_close_brace;
}

void enum_node::visit(ast_node_visitor& visitor) const
{
  visitor.visit(*this);
}

source_location enum_node::begin_location() const
{
  return m_enum_kw.src_range().begin;
}

source_location enum_node::end_location() const
{
  return m_semicolon.src_range().end;
}

const ast_node::token_t& enum_node::semicolon() const
{
  return m_semicolon;
}

bool enum_node::is_exported() const
{
  return m_export_kw.has_value();
}
}