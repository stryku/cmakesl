#pragma once

#include "common/string.hpp"

namespace cmsl {
namespace sema {
class sema_node;
}

namespace exec {
class module_sema_tree_provider
{
public:
  virtual const sema::sema_node& get_sema_tree(
    cmsl::string_view import_path) const = 0;
};
}
}
