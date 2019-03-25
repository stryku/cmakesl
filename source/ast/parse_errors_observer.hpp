#pragma once

#include "lexer/token/token.hpp"

#include "errors/errors_observer.hpp"
#include "errors/error.hpp"

namespace cmsl::ast
{
    class parse_errors_observer
    {
    public:
        virtual ~parse_errors_observer() = default;

        virtual void raise_unexpected_end_of_file(lexer::token::token token) = 0;
        virtual void raise_expected_token(lexer::token::token token, lexer::token::token_type type) = 0;
        virtual void raise_unexpected_token(lexer::token::token token) = 0;
        virtual void raise_expected_keyword(lexer::token::token token, lexer::token::token_type keyword) = 0;
        virtual void raise_expected_type(lexer::token::token token) = 0;
        virtual void raise_expected_parameter_declaration(lexer::token::token token) = 0;
        virtual void raise_expected_parameter(lexer::token::token token) = 0;
    };

    class parse_errors_reporter : public parse_errors_observer
    {
    public:
        explicit parse_errors_reporter(errors::errors_observer& errs_observer)
            : m_errors_observer{ errs_observer }
        {}

        void raise_unexpected_end_of_file(lexer::token::token token) override
        {
            m_errors_observer.nofify_error(errors::error{token.src_range(), "unexpected end of file"});
        }

        void raise_expected_token(lexer::token::token token, lexer::token::token_type type) override
        {
            m_errors_observer.nofify_error(errors::error{token.src_range(), "expected " + to_string(type)});
        }

        void raise_unexpected_token(lexer::token::token token) override
        {
            m_errors_observer.nofify_error(errors::error{token.src_range(), "unexpected token"});
        }

        void raise_expected_type(lexer::token::token token) override
        {
            m_errors_observer.nofify_error(errors::error{token.src_range(), "expected a type"});
        }

        void raise_expected_keyword(lexer::token::token token, lexer::token::token_type keyword) override
        {
            m_errors_observer.nofify_error(errors::error{token.src_range(), "expected " + to_string(keyword)});
        }

        void raise_expected_parameter_declaration(lexer::token::token token) override
        {
            m_errors_observer.nofify_error(errors::error{token.src_range(), "expected a parameter declaration"});
        }

        void raise_expected_parameter(lexer::token::token token) override
        {
            m_errors_observer.nofify_error(errors::error{token.src_range(), "expected a parameter"});
        }

    private:
        errors::errors_observer& m_errors_observer;
    };

    class parse_errors_sink : public parse_errors_observer
    {
    public:
        void raise_unexpected_end_of_file(lexer::token::token) override {}
        void raise_expected_token(lexer::token::token, lexer::token::token_type) override {}
        void raise_unexpected_token(lexer::token::token) override {}
        void raise_expected_type(lexer::token::token) override {}
        void raise_expected_keyword(lexer::token::token, lexer::token::token_type) override {}
        void raise_expected_parameter_declaration(lexer::token::token) override {}
        void raise_expected_parameter(lexer::token::token) override {}
    };
}
