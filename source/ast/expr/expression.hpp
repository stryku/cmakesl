#pragma once

#include <boost/variant.hpp>

namespace cmsl
{
    namespace ast
    {
        class expression
        {
        public:
            using result_t = boost::variant<int>;

            virtual ~expression() {}
            virtual result_t evaluate() = 0;
        };
    }
}
