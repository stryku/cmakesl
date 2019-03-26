#include "exec/source_executor.hpp"
#include "test/exec/mock/cmake_facade_mock.hpp"
#include "common/source_view.hpp"

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
                const auto result = executor.execute2(cmsl::source_view{ source });
                EXPECT_THAT(result, Eq(42));
            }

            TEST(SourceExecutorTest, UserFunction)
            {
                const auto source =
                    "int foo()\n"
                    "{\n"
                    "    return 42;\n"
                    "}\n"
                    "\n"
                    "int main()\n"
                    "{\n"
                    "    return foo();\n"
                    "}\n";

                cmake_facade_mock facade;
                source_executor executor{ facade };
                const auto result = executor.execute2(cmsl::source_view{ source });
                EXPECT_THAT(result, Eq(42));
            }

            TEST(SourceExecutorTest, BinaryOperator)
            {
                const auto source =
                        "int main()\n"
                        "{\n"
                        "    return 42 - 84;\n"
                        "}\n";

                cmake_facade_mock facade;
                source_executor executor{ facade };
                const auto result = executor.execute2(cmsl::source_view{ source });
                EXPECT_THAT(result, Eq(-42));
            }

            TEST(SourceExecutorTest, Class)
            {
                const auto source =
                        "class foo\n"
                        "{\n"
                        "    int value;\n"
                        "    int get_value() { return value; }\n"
                        "};\n"
                        "\n"
                        "int main()\n"
                        "{\n"
                        "    foo bar;\n"
                        "    bar.value = 42;\n"
                        "    return bar.get_value();\n"
                        "}\n";

                cmake_facade_mock facade;
                source_executor executor{ facade };
                const auto result = executor.execute2(cmsl::source_view{ source });
                EXPECT_THAT(result, Eq(42));
            }

            TEST(SourceExecutorTest, SymbolsLookup)
            {
                const auto source =
                        "int function()\n"
                        "{\n"
                        "    return 42;\n"
                        "}\n"
                        "\n"
                        "class foo\n"
                        "{\n"
                        "    int value;\n"
                        "    int function() { return value; }\n"
                        "    int call_function() { return function(); }\n"
                        "};\n"
                        "\n"
                        "foo bar;\n"
                        "\n"
                        "int main()\n"
                        "{\n"
                        "    foo bar;\n"
                        "    bar.value = 84;\n"
                        "    return function() - bar.call_function();\n"
                        "}\n";

                cmake_facade_mock facade;
                source_executor executor{ facade };
                const auto result = executor.execute2(cmsl::source_view{ source });
                EXPECT_THAT(result, Eq(-42));
            }
        }
    }
}
