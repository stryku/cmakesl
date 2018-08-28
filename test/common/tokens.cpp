#include "test/common/tokens.hpp"


namespace cmsl
{
    namespace test
    {
        namespace common
        {
            using token_type_t = lexer::token::token_type;
            using token_t = lexer::token::token;

            namespace details
            {
                token_t token_from_source(token_type_t type, cmsl::string_view source)
                {
                    const auto begin = source_location{};
                    source_location end;
                    end.line = 1u;
                    end.column = source.size();
                    end.absolute = source.size();
                    const auto source_range = cmsl::source_range{ begin, end };

                    return token_t{ type, source_range, source };
                }
            }

            token_t token_undef() { return token_t{ token_type_t::undef }; }
            token_t token_integer() { return token_t{ token_type_t::integer }; }
            token_t token_real() { return token_t{ token_type_t::real }; }
            token_t token_open_brace() { return token_t{ token_type_t::open_brace }; }
            token_t token_close_brace() { return token_t{ token_type_t::close_brace }; }
            token_t token_open_square() { return token_t{ token_type_t::open_square }; }
            token_t token_close_square() { return token_t{ token_type_t::close_square }; }
            token_t token_open_paren() { return token_t{ token_type_t::open_paren }; }
            token_t token_close_paren() { return token_t{ token_type_t::close_paren }; }
            token_t token_equal() { return token_t{ token_type_t::equal }; }
            token_t token_equalequal() { return token_t{ token_type_t::equalequal }; }
            token_t token_minus() { return token_t{ token_type_t::minus }; }
            token_t token_minusminus() { return token_t{ token_type_t::minusminus }; }
            token_t token_minusequal() { return token_t{ token_type_t::minusequal }; }
            token_t token_plusplus() { return token_t{ token_type_t::plusplus }; }
            token_t token_plusequal() { return token_t{ token_type_t::plusequal }; }
            token_t token_amp() { return token_t{ token_type_t::amp }; }
            token_t token_ampamp() { return token_t{ token_type_t::ampamp }; }
            token_t token_ampequal() { return token_t{ token_type_t::ampequal }; }
            token_t token_pipe() { return token_t{ token_type_t::pipe }; }
            token_t token_pipepipe() { return token_t{ token_type_t::pipepipe }; }
            token_t token_pipeequal() { return token_t{ token_type_t::pipeequal }; }
            token_t token_slash() { return token_t{ token_type_t::slash }; }
            token_t token_slashequal() { return token_t{ token_type_t::slashequal }; }
            token_t token_star() { return token_t{ token_type_t::star }; }
            token_t token_starequal() { return token_t{ token_type_t::starequal }; }
            token_t token_percent() { return token_t{ token_type_t::percent }; }
            token_t token_percentequal() { return token_t{ token_type_t::percentequal }; }
            token_t token_exclaim() { return token_t{ token_type_t::exclaim }; }
            token_t token_exclaimequal() { return token_t{ token_type_t::exclaimequal }; }
            token_t token_xor() { return token_t{ token_type_t::xor_ }; }
            token_t token_xorequal() { return token_t{ token_type_t::xorequal }; }
            token_t token_string() { return token_t{ token_type_t::string }; }
            token_t token_semicolon() { return token_t{ token_type_t::semicolon }; }
            token_t token_comma() { return token_t{ token_type_t::comma }; }

            token_t token_kw_if() { return token_t{ token_type_t::kw_if }; }
            token_t token_kw_else() { return token_t{ token_type_t::kw_else }; }

            token_t token_t_int() 
            {
                static const cmsl::string_view source = "int";
                return details::token_from_source(token_type_t::t_int, source);
            }

            token_t token_t_real()
            {
                static const auto source = "real";
                return details::token_from_source(token_type_t::t_real, source);
            }

            token_t token_integer(cmsl::string_view str)
            { 
                return details::token_from_source(token_type_t::integer, str);
            }

            token_t token_plus()
            {
                static const auto source = "+";
                return details::token_from_source(token_type_t::plus, source);
            }

            token_t token_identifier(cmsl::string_view str)
            {
                return details::token_from_source(token_type_t::identifier, str);
            }

            token_t token_dot()
            {
                static const auto source = ".";
                return details::token_from_source(token_type_t::dot, source);
            }
        }
    }
}