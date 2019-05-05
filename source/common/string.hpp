#pragma once

#include <string_view>
#include <unordered_map>

namespace cmsl {
using string_view = std::string_view;

template <typename Value>
using string_view_map = std::unordered_map<string_view, Value>;
}
