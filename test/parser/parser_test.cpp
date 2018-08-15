#include "ast/builtin_ast_context.hpp"
#include "ast/type.hpp"
#include "ast/parser.hpp"
#include "errors/errors_observer.hpp"
#include "lexer/lexer.hpp"
#include "lexer/token/token.hpp"

#include "test/common/tokens.hpp"

#include <gmock/gmock.h>

#include <sstream>

namespace cmsl
{
    namespace test
    {
        namespace parser
        {
            using lexer_t = cmsl::lexer::lexer;
            using token_t = cmsl::lexer::token::token;
            using token_type_t = cmsl::lexer::token::token_type;
            using tokens_container_t = cmsl::lexer::token::token_container_t;
            using parser_t = cmsl::ast::parser;
            using builtin_ctx_t = ast::builtin_ast_context;

            errors::errors_observer dummy_err_observer;

            using namespace cmsl::test::common;

            namespace get_type
            {
                namespace builtin
                {
                    using BuiltinTypeToken = testing::TestWithParam<tokens_container_t>;

                    TEST_P(BuiltinTypeToken, GetBuiltInType)
                    {
                        const auto tokens = GetParam();
                        auto p = parser_t{ dummy_err_observer, tokens };
                        builtin_ctx_t ctx;
                        const auto type = p.get_type(ctx);
                        ASSERT_THAT(type->is_builtin(), true);
                    }

                    const auto values = testing::Values(
                        tokens_container_t{ token_t_int() },
                        tokens_container_t{ token_t_real() }
                    );

                    INSTANTIATE_TEST_CASE_P(Parser_GetType, BuiltinTypeToken, values);
                }
            }

            namespace get_parameters_declaration
            {
                TEST(Parser_GetParametersDeclaration, Empty_GetEmpty)
                {
                    const auto tokens = tokens_container_t{
                        token_open_paren(),
                        token_close_paren()
                    };

                    auto p = parser_t{ dummy_err_observer, tokens };
                    builtin_ctx_t ctx;
                    const auto parameters = p.get_parameters_declaration(ctx);
                    const auto parameters_initialized = static_cast<bool>(parameters);
                    ASSERT_THAT(parameters_initialized, true);
                    ASSERT_THAT(parameters->size(), 0u);
                }


                TEST(Parser_GetParametersDeclaration, NotEmpty_GetParameters)
                {
                    const auto tokens = tokens_container_t{
                        token_open_paren(),
                        
                        token_t_int(),
                        token_identifier(),
                        token_semicolon(),
                        
                        token_t_real(),
                        token_identifier(),

                        token_close_paren()
                    };

                    auto p = parser_t{ dummy_err_observer, tokens };
                    builtin_ctx_t ctx;
                    const auto parameters = *p.get_parameters_declaration(ctx);
                    ASSERT_THAT(parameters.size(), 2u);

                    ASSERT_THAT(parameters[0].param_type->is_builtin(), true);
                    ASSERT_THAT(parameters[1].param_type->is_builtin(), true);
                }
            }
        }
    }
}
