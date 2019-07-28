#include "exec/identifiers_context.hpp"

#include <gmock/gmock.h>

namespace cmsl::exec::test {
class identifiers_context_mock : public identifiers_context
{
public:
  MOCK_METHOD1(lookup_identifier, inst::instance*(unsigned index));
  MOCK_METHOD0(get_class_instance, inst::instance*());
};
}