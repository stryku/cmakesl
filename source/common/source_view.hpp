#pragma once

#include "common/string.hpp"

namespace cmsl {
class source_view
{
public:
  struct line_info
  {
    cmsl::string_view line;
    cmsl::string_view::size_type start_pos;
  };

  explicit source_view(cmsl::string_view source);
  explicit source_view(cmsl::string_view source_path,
                       cmsl::string_view source);

  source_view(const source_view&) = default;
  source_view& operator=(const source_view&) = default;
  source_view(source_view&&) = default;
  source_view& operator=(source_view&&) = default;

  cmsl::string_view path() const;
  cmsl::string_view source() const;

  cmsl::string_view::const_iterator cbegin() const;
  cmsl::string_view::const_iterator cend() const;
  cmsl::string_view::const_pointer cdata() const;

  // Line numbers start from 1, not 0.
  line_info line(unsigned line_no) const;

private:
  cmsl::string_view m_path;
  cmsl::string_view m_source;
};
}
