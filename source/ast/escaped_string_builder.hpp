#pragma once

#include "common/string.hpp"

#include <string>

namespace cmsl::ast {
struct escaped_string_builder
{
  static std::string escape(cmsl::string_view str)
  {
    std::string result;

    for (auto i = 0u; i < str.size(); ++i) {
      const auto c = str.at(i);
      if (c == '\\') {
        ++i;
        if (i >= str.size()) {
          break;
        }

        const auto char_meant_to_be_escaped = str.at(i);

        const auto result_char = [char_meant_to_be_escaped] {
          switch (char_meant_to_be_escaped) {
            case 'n':
              return '\n';
            case 't':
              return '\t';
            case '\\':
              return '\\';
            default:
              return char_meant_to_be_escaped;
          }
        }();

        result += result_char;

        continue;
      }

      result += c;
    }

    return result;
  }
};
}
