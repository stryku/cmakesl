#include "lexer/token/token.hpp"

namespace cmsl
{
    namespace lexer
    {
        namespace token
        {
            token::token()
                : token{ token_type_t::undef }
            {}

            token::token(token_type_t type)
                : token{ type, source_range{}, cmsl::string_view{} }
            {}

            token::token(token_type_t type, const source_range& src_range, cmsl::string_view source)
                : m_type{ type }
                , m_source_range{ src_range }
                , m_source{ source }
            {}

            bool token::is_valid() const
            {
                return get_type() != token_type_t::undef;
            }

            token::token_type_t token::get_type() const
            {
                return m_type;
            }

            token token::undef()
            {
                return token{ token_type_t::undef };
            }

            cmsl::string_view token::str() const
            {
                return cmsl::string_view{
                    std::next(m_source.begin(), m_source_range.begin.absolute),
                    m_source_range.size()
                };
            }

            bool token::operator==(const token& rhs) const
            {
                return get_type() == rhs.get_type() &&
                    str() == rhs.str();
            }

            std::ostream& operator<<(std::ostream& out, const token& t)
            {
                out << '(' << static_cast<int>(t.get_type()) << ')' << t.str().to_string();
                return out;
            }

        }
    }
}
