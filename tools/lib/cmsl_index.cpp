#include "cmsl_index.hpp"

#include "cmsl_parsed_source.hpp"
#include "indexing_visitor.hpp"

#include <stack>
#include <vector>

struct cmsl_index_entries* cmsl_index(
  const struct cmsl_parsed_source* parsed_source)
{
  if (!parsed_source || !parsed_source->ast_tree ||
      !parsed_source->sema_tree) {
    return nullptr;
  }

  cmsl::tools::indexer indexer;
  parsed_source->sema_tree->visit(indexer);
  const auto& result = indexer.result();

  auto index_entries = new cmsl_index_entries;

  index_entries->num_entries = result.size();

  index_entries->entries = new cmsl_index_entry[result.size()];

  for (auto i = 0u; i < result.size(); ++i) {
    index_entries->entries[i] = result[i];
  }

  return index_entries;
}

void cmsl_destroy_index_entries(struct cmsl_index_entries* index_entries)
{
  for (auto i = 0u; i < index_entries->num_entries; ++i) {
    delete[] index_entries->entries[i].source_path;
  }

  delete[] index_entries->entries;
  delete index_entries;
}
