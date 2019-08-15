#include "sema/dumper.hpp"

#include <gmock/gmock.h>

#include <sstream>

namespace cmsl::sema::test {

TEST(SemaDumper, ImplementsAllPureVirtualMethods)
{
  std::ostringstream oss;
  dumper d{ oss };
}
}
