#ifndef CMSL_INDEX_HPP
#define CMSL_INDEX_HPP

#include "cmsl_parse_source.hpp"

#ifdef __cplusplus
extern "C" {
#endif
enum cmsl_index_entry_type
{
  type,
  identifier,
  parameter_declaration_identifier,
  class_member_identifier,
  operator_function,
  function_call_name,
  namespace_,
  enum_value,
  file,
  add_subdirectory
};

struct cmsl_index_entry
{
  unsigned begin_pos;
  unsigned end_pos;
  enum cmsl_index_entry_type type;
  char* source_path;
  unsigned position;
};

struct cmsl_index_entries
{
  struct cmsl_index_entry* entries;
  unsigned num_entries;
};

struct cmsl_index_entries* cmsl_index(
  const struct cmsl_parsed_source* parsed_source);
void cmsl_destroy_index_entries(struct cmsl_index_entries* index_entries);

#ifdef __cplusplus
}
#endif

#endif // CMSL_INDEX_HPP
