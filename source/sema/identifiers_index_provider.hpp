#pragma once

namespace cmsl::sema {
class identifiers_index_provider
{
public:
  using id_t = unsigned;

  static id_t get_next()
  {
    static id_t current{ 0u };
    return current++;
  }
};
}
