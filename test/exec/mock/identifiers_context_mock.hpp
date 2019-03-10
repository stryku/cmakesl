#include "exec/identifiers_context.hpp"

#include <gmock/gmock.h>

namespace cmsl
{
    namespace exec
    {
        namespace test
        {
            class identifiers_context_mock : public identifiers_context
            {
            public:
                MOCK_METHOD1(lookup_identifier, inst::instance*(cmsl::string_view));
            };
        }
    }
}