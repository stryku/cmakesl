#include "exec/instance/instance.hpp"
#include "sema/sema_type.hpp"

#include <gmock/gmock.h>

namespace cmsl::exec::inst::test {
class instance_mock : public instance
{
private:
public:
  MOCK_CONST_METHOD0(copy, std::unique_ptr<instance>());

  MOCK_CONST_METHOD0(value, instance_value_variant());
  MOCK_METHOD0(value_ref, instance_value_variant&());
  MOCK_CONST_METHOD0(value_cref, const instance_value_variant&());
  MOCK_METHOD1(assign, void(instance_value_variant));
  MOCK_METHOD2(assign_member, void(unsigned, std::unique_ptr<instance>));
  MOCK_METHOD1(find_member, instance*(unsigned));
  MOCK_CONST_METHOD1(find_cmember, const instance*(unsigned));
  MOCK_CONST_METHOD1(
    find_function, sema::single_scope_function_lookup_result_t(lexer::token));
  MOCK_CONST_METHOD0(type, const sema::sema_type&());
};
}