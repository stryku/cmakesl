#include "exec/source_executor.hpp"
#include "test/exec/mock/cmake_facade_mock.hpp"

#include <gmock/gmock.h>

namespace cmsl
{
    namespace exec
    {
        namespace test
        {
            using ::testing::Eq;

            TEST(SourceExecutorTest, FunctionReturn)
            {
                const auto source = "int main() { return 42; }";
                cmake_facade_mock facade;
                source_executor executor{ facade };
                const auto result = executor.execute2(source);
                EXPECT_THAT(result, Eq(42));
            }
        }
    }
}
