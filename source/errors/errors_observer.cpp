#include "errors/errors_observer.hpp"
#include "error.hpp"

#include <iostream>

namespace cmsl
{
    namespace errors
    {
        void errors_observer::nofify_error(const error& error)
        {
            std::cout << error.range.begin.line << ':' << error.range.begin.column << ": " << error.message << "\n";
        }
    }
}
