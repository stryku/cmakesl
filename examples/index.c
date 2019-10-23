#include "lib/cmsl_index.hpp"

#include <stdio.h>

const char* entry_type_to_string(enum cmsl_index_entry_type entry_type)
{
  switch (entry_type) {

    case type:
      return "type";
    case identifier:
      return "identifier";
    case parameter_declaration_identifier:
      return "parameter declaration identifier";
    case class_member_identifier:
      return "class member identifier";
    case operator_function:
      return "operator function";
    case function_call_name:
      return "function call name";
    case namespace_:
      return "namespace name";
    case enum_value:
      return "enum value";
    default:
      return "unknown";
  }
}

void print_line_snippet(const char* source, struct cmsl_index_entry* entry)
{
  unsigned line_start = entry->begin_pos;
  while (line_start > 0u && source[line_start] != '\n') {
    --line_start;
  }

  unsigned print_pos = (line_start > 0) ? line_start + 1 : line_start;

  while (source[print_pos] != '\0' && source[print_pos] != '\n') {
    putchar(source[print_pos]);
    ++print_pos;
  }

  putchar('\n');

  int i;
  for (i = line_start; i < entry->begin_pos - 1; ++i) {
    putchar(' ');
  }
  for (i = entry->begin_pos; i < entry->end_pos; ++i) {
    putchar('^');
  }

  putchar('\n');
}

int main(int argc, const char* argv[])
{
  const char* builtin_types_documentation_path = NULL;

  if (argc > 1) {
    builtin_types_documentation_path = argv[1];
  }

  // clang-format off
  const char* source = "class hello_world\n"
                       "{\n"
                       "    int foo_getter()\n"
                       "    {\n"
                       "        return foo;\n"
                       "    }\n"
                       "\n"
                       "    int foo;\n"
                       "};\n"
                       "\n"
                       "int foo() { return 0; }\n"
                       "\n"
                       "int bar;\n"
                       "\n"
                       "int main(int param_1, list<string> param_2)\n"
                       "{\n"
                       "    hello_world hw;\n"
                       "    hw.foo = param_1;\n"
                       "    return hw.foo_getter();\n"
                       "}\n";
  // clang-format on

  struct cmsl_parsed_source* parsed_source =
    cmsl_parse_source(source, builtin_types_documentation_path);
  if (!parsed_source) {
    return 1;
  }

  struct cmsl_index_entries* index_entries = cmsl_index(parsed_source);
  if (!index_entries) {
    cmsl_destroy_parsed_source(parsed_source);
    return 2;
  }

  int i;
  for (i = 0; i < index_entries->num_entries; ++i) {
    struct cmsl_index_entry* entry = &index_entries->entries[i];
    const unsigned size = entry->end_pos - entry->begin_pos;

    printf("entry type: %s, source begin: %d, source end: %d, destination "
           "position: %d, destination path: %s\n",
           entry_type_to_string(entry->type), entry->begin_pos, entry->end_pos,
           entry->position, entry->source_path);
    print_line_snippet(source, entry);
  }

  cmsl_destroy_index_entries(index_entries);
  cmsl_destroy_parsed_source(parsed_source);

  return 0;
}
