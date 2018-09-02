#include "exec/source_executor.hpp"

#include <gmock/gmock.h>

namespace cmsl
{
    namespace test
    {
        namespace high_level_exec
        {
            using source_executor_t = cmsl::exec::source_executor;

            TEST(Exec, VariableAssign)
            {
                const auto source =
                    "int main()"
                    "{"
                    "    int foo;"
                    "    foo = 2;"
                    "    return foo;"
                    "}";

                const auto result = source_executor_t{}.execute(source);
                ASSERT_THAT(result, 2);
            }

            TEST(Exec, VariableSelfAssign)
            {
                const auto source =
                    "int main()"
                    "{"
                    "    int foo = 2;"
                    "    foo = foo + 2;"
                    "    return foo;"
                    "}";

                const auto result = source_executor_t{}.execute(source);
                ASSERT_THAT(result, 4);
            }

            TEST(Exec, ClassMemberAssign)
            {
                const auto source =
                    "class Foo"
                    "{"
                    "    int m_bar;"
                    "};"
                    ""
                    "int main()"
                    "{"
                    "    Foo foo;"
                    "    foo.m_bar = 2;"
                    "    return foo.m_bar;"
                    "}";

                const auto result = source_executor_t{}.execute(source);
                ASSERT_THAT(result, 2);
            }

            TEST(Exec, ClassMemberFunction)
            {
                const auto source =
                    "class Foo"
                    "{"
                    "    int bar()"
                    "    {"
                    "        return 2;"
                    "    }"
                    "};"
                    ""
                    "int main()"
                    "{"
                    "    Foo foo;"
                    "    return foo.bar();"
                    "}";

                const auto result = source_executor_t{}.execute(source);
                ASSERT_THAT(result, 2);
            }

            TEST(Exec, ClassMemberFunctionUsingClassMember)
            {
                const auto source =
                    "class Foo"
                    "{"
                    "    int bar()"
                    "    {"
                    "        return m_qux / m_baz;"
                    "    }"
                    ""
                    "    int m_baz;"
                    "    int m_qux;"
                    "};"
                    ""
                    "int main()"
                    "{"
                    "    Foo foo;"
                    "    foo.m_baz = 2;"
                    "    foo.m_qux = 10;"
                    "    return foo.bar();"
                    "}";

                const auto result = source_executor_t{}.execute(source);
                ASSERT_THAT(result, 5);
            }

            TEST(Exec, UserTypeClassMember)
            {
                const auto source =
                    "class Foo"
                    "{"
                    "    int m_bar;"
                    "};"
                    ""
                    "class Baz"
                    "{"
                    "    Foo m_foo;"
                    "};"
                    ""
                    "int main()"
                    "{"
                    "    Baz baz;"
                    "    baz.m_foo.m_bar = 2;"
                    "    return baz.m_foo.m_bar;"
                    "}";

                const auto result = source_executor_t{}.execute(source);
                ASSERT_THAT(result, 2);
            }
        }
    }
}