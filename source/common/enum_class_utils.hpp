#pragma once

#include <unordered_map>
#include <unordered_set>

namespace cmsl {
namespace details {
struct enum_class_hash
{
  template <typename T>
  std::size_t operator()(T t) const
  {
    return static_cast<std::size_t>(t);
  }
};
}

template <typename K, typename V>
using enum_unordered_map = std::unordered_map<K, V, details::enum_class_hash>;

template <typename V>
using enum_unordered_set = std::unordered_set<V, details::enum_class_hash>;
}
