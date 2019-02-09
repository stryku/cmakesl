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

            TEST(SourceExecutorTest, UserFunction)
            {
                const auto source =
                    "int foo()"
                    "{"
                    "    return 42;"
                    "}"
                    ""
                    "int main()"
                    "{"
                    "    return foo();"
                    "}";

                cmake_facade_mock facade;
                source_executor executor{ facade };
                const auto result = executor.execute2(source);
                EXPECT_THAT(result, Eq(42));
            }

            TEST(SourceExecutorTest, BinaryOperator)
            {
                const auto source =
                        "int main()"
                        "{"
                        "    return 42 - 84;"
                        "}";

                cmake_facade_mock facade;
                source_executor executor{ facade };
                const auto result = executor.execute2(source);
                EXPECT_THAT(result, Eq(-42));
            }

            TEST(SourceExecutorTest, Class)
            {
                const auto source =
                        "class foo"
                        "{"
                        "    int value;"
                        "    int get_value() { return value; }"
                        "};"
                        ""
                        "int main()"
                        "{"
                        "    foo bar;"
                        "    bar.value = 42;"
                        "    return foo.get_value();"
                        "}";

                cmake_facade_mock facade;
                source_executor executor{ facade };
                const auto result = executor.execute2(source);
                EXPECT_THAT(result, Eq(42));
            }
        }
    }
}
