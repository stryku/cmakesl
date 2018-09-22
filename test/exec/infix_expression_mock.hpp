#include "exec/infix/infix_expression.hpp"

#include <gmock/gmock.h>

namespace cmsl
{
    namespace test
    {
        namespace exec
        {
            namespace infix
            {
                class infix_expression_mock : public cmsl::exec::infix::infix_expression
                {
                private:
                    using instance_t = cmsl::exec::inst::instance;
                    using infix_evaluation_context_t = cmsl::exec::infix::infix_evaluation_context;

                public:
                    MOCK_CONST_METHOD1(evaluate, instance_t*(infix_evaluation_context_t&));
                };
            }
        }
    }
}
