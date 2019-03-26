#include "errors/errors_observer.hpp"
#include "error.hpp"

#include <iostream>

namespace
{
    cmsl::string_view to_string(cmsl::errors::error_type type)
    {
        switch (type)
        {
            case cmsl::errors::error_type::error: return "error";
            case cmsl::errors::error_type::warning: return "warning";
            case cmsl::errors::error_type::note: return "note";
        }
    }
}

namespace cmsl
{
    namespace errors
    {
        void errors_observer::nofify_error(const error& error)
        {
            std::cout << error.source_path << ':'
            << error.range.begin.line << ':'
            << error.range.begin.column << ": "
            << to_string(error.type) << ": "
            << error.message << '\n'
            << error.line_snippet << '\n';
        }
    }
}
