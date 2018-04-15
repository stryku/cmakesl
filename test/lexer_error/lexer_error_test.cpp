#include "lexer/lexer.hpp"
#include "lexer/token/token.hpp"
#include "errors/errors_observer.hpp"
#include "errors_observer_mock/errors_observer_mock.hpp"

#include <gmock/gmock.h>

#include <sstream>

namespace cmsl
{
    namespace test
    {
        namespace lexer_error
        {
            using lexer_t = cmsl::lexer::lexer;
            using token_type_t = cmsl::lexer::token::token_type;

            TEST(LexerErrorTest, Empty_NoError)
            {
                testing::StrictMock<errors_observer::errors_observer_mock> err_observer_mock;
                errors::errors_observer err_observer;
                const auto source = "";
                cmsl::lexer::lexer lex{ err_observer, source };
                (void)lex.lex();
                // Assume no calls to error observer
            }
        }
    }
}
