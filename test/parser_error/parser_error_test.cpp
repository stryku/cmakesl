#include "lexer/token/token.hpp"
#include "errors/errors_observer.hpp"
#include "errors_observer_mock/errors_observer_mock.hpp"
#include "ast/parser.hpp"

#include <gmock/gmock.h>

#include <sstream>

using namespace testing;

namespace cmsl
{
    namespace test
    {
        namespace parser_error
        {
            using token_t = cmsl::lexer::token::token;
            using token_type_t = cmsl::lexer::token::token_type;
            using token_container_t = cmsl::lexer::token::token_container_t;
            using parser_t = cmsl::ast::parser;

            namespace get_type
            {
                namespace builtin_or_identifier
                {
                    using BuiltinOrIdentifier = testing::TestWithParam<token_container_t>;

                    TEST_P(BuiltinOrIdentifier, NoError)
                    {
                        testing::StrictMock<errors_observer::errors_observer_mock> err_observer_mock;
                        errors::errors_observer err_observer;
                        const auto tokens = GetParam();
                        auto p = parser_t{ err_observer, tokens };
                        (void)p.get_type();
                    }

                    const auto values = testing::Values(
                        token_container_t{ token_t{ token_type_t::t_int } },
                        token_container_t{ token_t{ token_type_t::t_real } },
                        token_container_t{ token_t{ token_type_t::identifier } }
                    );

                    INSTANTIATE_TEST_CASE_P(ParserError_GetType, BuiltinOrIdentifier, values);
                }

                namespace unexpected_token
                {
                    using UnexpectedToken = testing::TestWithParam<token_container_t>;

                    TEST_P(UnexpectedToken, ReportError)
                    {
                        testing::StrictMock<errors_observer::errors_observer_mock> err_observer_mock;
                        errors::errors_observer err_observer;
                        const auto tokens = GetParam();

                        EXPECT_CALL(err_observer_mock, notify_error(_)).Times(1);

                        auto p = parser_t{ err_observer, tokens };
                        (void)p.get_type();
                    }

                    const auto values = testing::Values(
                        token_container_t{ token_t{ token_type_t::integer } },
                        token_container_t{ token_t{ token_type_t::real } },
                        token_container_t{ token_t{ token_type_t::dot } },
                        token_container_t{ token_t{ token_type_t::open_brace } },
                        token_container_t{ token_t{ token_type_t::close_brace } },
                        token_container_t{ token_t{ token_type_t::open_square } },
                        token_container_t{ token_t{ token_type_t::close_square } },
                        token_container_t{ token_t{ token_type_t::open_paren } },
                        token_container_t{ token_t{ token_type_t::close_paren } },
                        token_container_t{ token_t{ token_type_t::equal } },
                        token_container_t{ token_t{ token_type_t::equalequal } },
                        token_container_t{ token_t{ token_type_t::minus } },
                        token_container_t{ token_t{ token_type_t::minusminus } },
                        token_container_t{ token_t{ token_type_t::minusequal } },
                        token_container_t{ token_t{ token_type_t::plus } },
                        token_container_t{ token_t{ token_type_t::plusplus } },
                        token_container_t{ token_t{ token_type_t::plusequal } },
                        token_container_t{ token_t{ token_type_t::amp } },
                        token_container_t{ token_t{ token_type_t::ampamp } },
                        token_container_t{ token_t{ token_type_t::ampequal } },
                        token_container_t{ token_t{ token_type_t::pipe } },
                        token_container_t{ token_t{ token_type_t::pipepipe } },
                        token_container_t{ token_t{ token_type_t::pipeequal } },
                        token_container_t{ token_t{ token_type_t::slash } },
                        token_container_t{ token_t{ token_type_t::slashequal } },
                        token_container_t{ token_t{ token_type_t::star } },
                        token_container_t{ token_t{ token_type_t::starequal } },
                        token_container_t{ token_t{ token_type_t::percent } },
                        token_container_t{ token_t{ token_type_t::percentequal } },
                        token_container_t{ token_t{ token_type_t::exclaim } },
                        token_container_t{ token_t{ token_type_t::exclaimequal } },
                        token_container_t{ token_t{ token_type_t::xor } },
                        token_container_t{ token_t{ token_type_t::xorequal } },
                        token_container_t{ token_t{ token_type_t::string } },
                        token_container_t{ token_t{ token_type_t::semicolon } }
                    );

                    INSTANTIATE_TEST_CASE_P(ParserError_GetType, UnexpectedToken, values);
                }
            }

            namespace get_parameters_declaration
            {
                namespace missing_parameter_declaration
                {
                    auto parameters(const token_container_t& list_tokens)
                    {
                        token_container_t result{ token_t{ token_type_t::open_paren } };

                        std::copy(std::cbegin(list_tokens), std::cend(list_tokens),
                                  std::back_inserter(result));

                        result.emplace_back(token_t{ token_type_t::close_paren });

                        return result;
                    }

                    using MissingParameterDeclaration = testing::TestWithParam<token_container_t>;

                    TEST_P(MissingParameterDeclaration, ReportError)
                    {
                        testing::StrictMock<errors_observer::errors_observer_mock> err_observer_mock;
                        errors::errors_observer err_observer;
                        const auto tokens = GetParam();

                        EXPECT_CALL(err_observer_mock, notify_error(_)).Times(1);

                        auto p = parser_t{ err_observer, tokens };
                        (void)p.get_parameters_declaration();
                    }

                    const auto values = testing::Values(
                        parameters({ token_t{ token_type_t::semicolon } }), // (,)
                        parameters({ token_t{ token_type_t::identifier }, token_t{ token_type_t::identifier }, token_t{ token_type_t::semicolon } }), //(type name,)
                        parameters({ token_t{ token_type_t::semicolon }, token_t{ token_type_t::identifier }, token_t{ token_type_t::identifier } }), //(,type name)
                        parameters({ token_t{ token_type_t::identifier }, token_t{ token_type_t::identifier }, token_t{ token_type_t::semicolon }, token_t{ token_type_t::semicolon }, token_t{ token_type_t::identifier }, token_t{ token_type_t::identifier } }) //(type name,,type name)
                    );

                    INSTANTIATE_TEST_CASE_P(ParserError_GetParametersDeclaration, MissingParameterDeclaration, values);
                }

                namespace missing_parenthesis
                {
                    using MissingParenthesis = testing::TestWithParam<token_container_t>;

                    TEST_P(MissingParenthesis, ReportError)
                    {
                        testing::StrictMock<errors_observer::errors_observer_mock> err_observer_mock;
                        errors::errors_observer err_observer;
                        const auto tokens = GetParam();

                        EXPECT_CALL(err_observer_mock, notify_error(_)).Times(1);

                        auto p = parser_t{ err_observer, tokens };
                        (void)p.get_parameters_declaration();
                    }

                    const auto values = testing::Values(
                        token_container_t{ token_t{ token_type_t::open_paren } },
                        token_container_t{ token_t{ token_type_t::close_paren } },
                        token_container_t{ token_t{ token_type_t::open_paren }, token_t{ token_type_t::identifier },token_t{ token_type_t::identifier } },
                        token_container_t{ token_t{ token_type_t::open_paren }, token_t{ token_type_t::identifier },token_t{ token_type_t::identifier }, token_t{ token_type_t::semicolon } },
                        token_container_t{ token_t{ token_type_t::identifier },token_t{ token_type_t::identifier }, token_t{ token_type_t::close_paren } },
                        token_container_t{ token_t{ token_type_t::identifier },token_t{ token_type_t::identifier }, token_t{ token_type_t::semicolon }, token_t{ token_type_t::close_paren } }
                    );

                    INSTANTIATE_TEST_CASE_P(ParserError_GetParametersDeclaration, MissingParenthesis, values);
                }
            }
        }
    }
}