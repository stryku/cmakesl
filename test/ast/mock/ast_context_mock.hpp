#pragma once

#include "ast/ast_context.hpp"

#include <gmock/gmock.h>

namespace cmsl
{
    namespace test
    {
        namespace ast
        {
            class ast_context_mock : public cmsl::ast::ast_context
            {
            private:
                using type_t = cmsl::ast::type;
                using function_t = cmsl::ast::function;

            public:

                MOCK_METHOD1(add_type, void(std::unique_ptr<type_t>));
                MOCK_CONST_METHOD1(find_type, const type_t*(cmsl::string_view));

                MOCK_METHOD1(add_function, void(std::unique_ptr<function_t>));
                MOCK_CONST_METHOD1(find_function, const function_t*(cmsl::string_view));
            };
        }
    }
}
