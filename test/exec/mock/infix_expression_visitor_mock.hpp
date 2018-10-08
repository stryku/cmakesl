#pragma once

#include "exec/infix/infix_expression_visitor.hpp"

#include <gmock/gmock.h>

namespace cmsl
{
    namespace exec
    {
        namespace infix
        {
            namespace test
            {
                class infix_expression_visitor_mock : public infix_expression_visitor
                {
                public:
                    MOCK_METHOD1(visit, void(
                            const bool_value_expression&));

                    MOCK_METHOD1(visit, void(
                            const int_value_expression&));

                    MOCK_METHOD1(visit, void(
                            const double_value_expression&));

                    MOCK_METHOD1(visit, void(
                            const string_value_expression&));

                    MOCK_METHOD1(visit, void(
                            const id_expression&));

                    MOCK_METHOD1(visit, void(
                            const member_function_call_expression&));

                    MOCK_METHOD1(visit, void(
                            const binary_operator_expression&));

                    MOCK_METHOD1(visit, void(
                            const function_call_expression&));

                    MOCK_METHOD1(visit, void(
                            const class_member_access_expression&));
                };
            }
        }
    }
}
