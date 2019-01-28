#include "exec/instance/instance.hpp"
#include "sema/sema_type.hpp"

#include <gmock/gmock.h>

namespace cmsl
{
    namespace exec
    {
        namespace inst
        {
            namespace test
            {
                class instance_mock : public instance
                {
                private:

                public:
                    MOCK_CONST_METHOD0(copy, std::unique_ptr<instance>());
                    MOCK_CONST_METHOD0(get_value, instance_value_t());
                    MOCK_METHOD0(get_value_ref, instance_value_t & ());
                    MOCK_CONST_METHOD0(get_value_cref, const instance_value_t & ());
                    MOCK_METHOD1(assign, void(instance_value_t));
                    MOCK_METHOD1(get_member, instance* (cmsl::string_view));
                    MOCK_CONST_METHOD1(get_cmember, const instance* (cmsl::string_view));
                    MOCK_CONST_METHOD1(has_function, bool(cmsl::string_view));
                    MOCK_CONST_METHOD1(is_ctor, bool(cmsl::string_view));
                    MOCK_CONST_METHOD1(get_function, const cmsl::ast::function *(cmsl::string_view));
                    MOCK_CONST_METHOD0(get_type, const cmsl::ast::type&());
                    MOCK_CONST_METHOD1(get_sema_function, const sema::sema_function *(cmsl::string_view));
                    MOCK_CONST_METHOD0(get_sema_type, const sema::sema_type&());
                };
            }
        }
    }
}