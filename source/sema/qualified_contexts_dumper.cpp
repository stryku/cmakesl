#include "sema/qualified_contexts_dumper.hpp"
#include "sema/enum_values_context.hpp"
#include "sema/functions_context.hpp"
#include "sema/identifiers_context.hpp"
#include "sema/qualified_entries_finder.hpp"
#include "sema/sema_type.hpp"
#include "sema/types_context.hpp"

namespace cmsl::sema {

void qualified_contexts_dumper::enter_global_scope(cmsl::string_view name)
{
  out() << "-enter scope: " << name;
  push_ident();
}

void qualified_contexts_dumper::leave_global_scope()
{
  pop_ident();
}

void qualified_contexts_dumper::dump(const qualified_contextes& ctxs)
{
  {
    out() << "-enums:";
    auto ig = ident();
    ctxs.enums->dump(*this);
  }
  {
    out() << "-functions:";
    auto ig = ident();
    ctxs.functions->dump(*this);
  }
  {
    out() << "-ids:";
    auto ig = ident();
    ctxs.ids->dump(*this);
  }
  {
    out() << "-types:";
    auto ig = ident();
    ctxs.types->dump(*this);
  }
}

void qualified_contexts_dumper::dump_enums(
  const qualified_entries_finder<enum_value_info>& entries)
{
  const auto entry_dumper = [this](const lexer::token& token, bool is_exported,
                                   const enum_value_info& entry) {
    out() << "name: " << token.str() << ", value: " << entry.value
          << ", exported: " << (is_exported ? "true" : "false")
          << ", index: " << entry.index
          << ", type: " << entry.type.get().fully_qualified_name();
  };

  entries.visit_entries(*this, entry_dumper);
}

void qualified_contexts_dumper::dump_functions(
  const qualified_entries_finder<function_data>& entries)
{
  const auto entry_dumper = [this](const lexer::token& token, bool is_exported,
                                   const function_data& entry) {
    out() << "name: " << token.str()
          << ", exported: " << (is_exported ? "true" : "false");
  };

  entries.visit_entries(*this, entry_dumper);
}

void qualified_contexts_dumper::dump_types(
  const qualified_entries_finder<type_data>& entries)
{
  const auto entry_dumper = [this](const lexer::token& token, bool is_exported,
                                   const type_data& entry) {
    out() << "name: " << token.str() << ", ptr: " << &entry.ty
          << ", exported: " << (is_exported ? "true" : "false")
          << ", full name: " << entry.ty.fully_qualified_name();
  };

  entries.visit_entries(*this, entry_dumper);
}

void qualified_contexts_dumper::dump_ids(
  const qualified_entries_finder<identifier_info>& entries)
{
  const auto entry_dumper = [this](const lexer::token& token, bool is_exported,
                                   const identifier_info& entry) {
    out() << "name: " << token.str()
          << ", exported: " << (is_exported ? "true" : "false")
          << ", type ptr: " << &entry.type.get()
          << ", full name: " << entry.type.get().fully_qualified_name();
  };

  entries.visit_entries(*this, entry_dumper);
}
}
