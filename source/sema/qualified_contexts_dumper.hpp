#pragma once

#include "common/dumper_utils.hpp"
#include "common/string.hpp"
#include "sema/qualified_contextes.hpp"

namespace cmsl::sema {
struct enum_value_info;
struct function_data;
struct type_data;
struct identifier_info;

template <typename>
class qualified_entries_finder;

class qualified_contexts_dumper : public dumper_utils
{
public:
  qualified_contexts_dumper(std::ostream& out)
    : dumper_utils{ out }
  {
  }

  template <typename Entry>
  void dump(const qualified_entries_finder<Entry>& entries)
  {
    if constexpr (std::is_same_v<Entry, enum_value_info>) {
      dump_enums(entries);
    } else if constexpr (std::is_same_v<Entry, function_data>) {
      dump_functions(entries);
    } else if constexpr (std::is_same_v<Entry, type_data>) {
      dump_types(entries);
    } else if constexpr (std::is_same_v<Entry, identifier_info>) {
      dump_ids(entries);
    }
  }

  void enter_global_scope(cmsl::string_view name);
  void leave_global_scope();

  void dump(const qualified_contextes& ctxs);

private:
  void dump_functions(const qualified_entries_finder<function_data>& entries);
  void dump_types(const qualified_entries_finder<type_data>& entries);
  void dump_enums(const qualified_entries_finder<enum_value_info>& entries);
  void dump_ids(const qualified_entries_finder<identifier_info>& entries);
};
}
