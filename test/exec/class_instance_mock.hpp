#include "exec/instance/instance.hpp"

#include <gmock/gmock.h>

namespace cmsl
{
    namespace test
    {
        namespace exec
        {
            class class_instance_mock : public cmsl::exec::inst::instance
            {
            private:
                using instance_t = cmsl::exec::inst::instance;
                using instance_value_t = cmsl::exec::inst::instance_value_t;

            public:
                MOCK_CONST_METHOD0(copy, std::unique_ptr<instance_t>());
                MOCK_CONST_METHOD0(get_value, instance_value_t());
                MOCK_METHOD0(get_value_ref, instance_value_t&());
                MOCK_METHOD1(assign, void(instance_value_t));
                MOCK_METHOD1(get_member, instance_t*(cmsl::string_view));
                MOCK_CONST_METHOD1(has_function, bool(cmsl::string_view));
                MOCK_CONST_METHOD1(is_ctor, bool(cmsl::string_view));
                MOCK_CONST_METHOD1(get_function, const ast::function*(cmsl::string_view));
                MOCK_CONST_METHOD0(get_type, const ast::type&());
            };
        }
    }
}
