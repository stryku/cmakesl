#pragma once

#include "exec/infix/infix_expression_visitor.hpp"

#include <gmock/gmock.h>

namespace cmsl
{
    namespace test
    {
        namespace exec
        {
            class infix_expression_visitor_mock : public cmsl::exec::infix::infix_expression_visitor
            {
            public:
                MOCK_METHOD1(visit, void(const cmsl::exec::infix::bool_value_expression&));
                MOCK_METHOD1(visit, void(const cmsl::exec::infix::int_value_expression&));
                MOCK_METHOD1(visit, void(const cmsl::exec::infix::double_value_expression&));
                MOCK_METHOD1(visit, void(const cmsl::exec::infix::string_value_expression&));
                MOCK_METHOD1(visit, void(const cmsl::exec::infix::id_expression&));
                MOCK_METHOD1(visit, void(const cmsl::exec::infix::member_function_call_expression&));
                MOCK_METHOD1(visit, void(const cmsl::exec::infix::binary_operator_expression&));
                MOCK_METHOD1(visit, void(const cmsl::exec::infix::function_call_expression&));
                MOCK_METHOD1(visit, void(const cmsl::exec::infix::class_member_access_expression&));
            };
        }
    }
}
