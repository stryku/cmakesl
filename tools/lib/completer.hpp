#pragma once

struct cmsl_parsed_source;
struct cmsl_complete_results;

namespace cmsl::tools {
class completer
{
public:
  explicit completer(const cmsl_parsed_source& parsed_source,
                     unsigned absolute_position);

  cmsl_complete_results* complete();

private:
  const cmsl_parsed_source& m_parsed_source;
  unsigned m_absolute_position;
};
}
