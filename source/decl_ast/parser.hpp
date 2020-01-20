#pragma once

#include "decl_ast/parse_errors_reporter.hpp"
#include "lexer/token.hpp"
#include "parse/parser_utils.hpp"

#include <memory>

namespace cmsl {
class strings_container;

namespace errors {
class errors_observer;
}

namespace decl_ast {
class ast_node;
class cmake_variable_access_node;
class component_declaration_node;
class component_node;
class import_node;
class list_node;
class property_access_node;
class property_node;
class translation_unit_node;

class parser : public parse::parser_utils
{
public:
  parser(errors::errors_observer& err_observer,
         strings_container& strings_container, cmsl::source_view source,
         const lexer::token_container_t& tokens);

  std::unique_ptr<translation_unit_node> parse_translation_unit();
  std::unique_ptr<component_declaration_node> parse_component_declaration();
  std::unique_ptr<component_node> parse_component();
  std::unique_ptr<ast_node> parse_property();
  std::unique_ptr<ast_node> parse_value();
  std::unique_ptr<list_node> parse_list();
  std::unique_ptr<cmake_variable_access_node> parse_cmake_variable_access();
  std::unique_ptr<import_node> parse_import();

private:
  std::optional<std::vector<std::unique_ptr<ast_node>>>
  parse_component_nodes();

  bool component_declaration_starts() const;
  bool property_definition_starts() const;

  std::unique_ptr<property_access_node> parse_property_access();

private:
  parse_errors_reporter m_errors_reporter;
};

}
}