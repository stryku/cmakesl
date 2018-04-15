#pragma once

namespace cmsl
{
    namespace errors
    {
        struct error;

        class errors_observer
        {
        public:
            void nofify_error(const error& error);
        };
    }
}
