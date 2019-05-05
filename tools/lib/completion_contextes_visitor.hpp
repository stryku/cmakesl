#pragma once

#include "completion_contextes.hpp"

struct cmsl_parsed_source;
struct cmsl_complete_results;

namespace cmsl::tools {
class standalone_expression_context;
class could_not_find_context;

class completion_contextes_visitor
{
public:
  explicit completion_contextes_visitor(
    const cmsl_parsed_source& parsed_source, cmsl_complete_results& results);

  void operator()(const could_not_find_context&) {}
  void operator()(const standalone_expression_context& ctx);
  void operator()(const top_level_declaration_context& ctx);
  void operator()(const class_member_declaration_context& ctx);

  void finalize();

private:
  void add_standalone_expression_keywords();
  void add_top_level_declaration_keywords();

  template <typename Collection>
  void add_results(Collection&& results);

private:
  const cmsl_parsed_source& m_parsed_source;
  cmsl_complete_results& m_results;

  std::vector<std::string> m_intermediate_results;
};
}
