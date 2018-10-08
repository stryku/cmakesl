#include "exec/infix/infix_expression.hpp"

#include <gmock/gmock.h>

namespace cmsl
{
    namespace exec
    {
        namespace infix
        {
            namespace test
            {
                class infix_expression_mock : public infix_expression
                {
                private:
                    using instance_t = inst::instance;

                public:
                    MOCK_CONST_METHOD1(evaluate, instance_t *(infix_evaluation_context
                            &));

                    MOCK_CONST_METHOD1(visit, void(infix_expression_visitor
                            &));
                };
            }
        }
    }
}