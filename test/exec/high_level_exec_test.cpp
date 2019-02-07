#include "exec/source_executor.hpp"

#include "cmake_facade.hpp"

#include <gmock/gmock.h>

namespace cmsl
{
    namespace test
    {
        namespace high_level_exec
        {
            using source_executor_t = cmsl::exec::source_executor;

            class fake_facade : public facade::cmake_facade
            {
            public:
                version get_cmake_version() const override
                {
                    return {};
                }
                void fatal_error(const std::string&) const override
                {}
                void register_project(const std::string&) override
                {}


                std::string get_current_binary_dir() const override
                {
                    return {};
                }
                std::string get_current_source_dir() const override
                {
                    return {};
                }

                void add_executable(const std::string& name, const std::vector<std::string>& sources) override
                {
                }
            };

            fake_facade facade;

            TEST(Exec, DISABLED_VariableAssign)
            {
                const auto source =
                    "int main()"
                    "{"
                    "    int foo;"
                    "    foo = 2;"
                    "    return foo;"
                    "}";

                const auto result = source_executor_t{facade}.execute(source);
                EXPECT_THAT(result, 2);
            }

            TEST(Exec, DISABLED_VariableSelfAssign)
            {
                const auto source =
                    "int main()"
                    "{"
                    "    int foo = 2;"
                    "    foo = foo + 2;"
                    "    return foo;"
                    "}";

                const auto result = source_executor_t{facade}.execute(source);
                EXPECT_THAT(result, 4);
            }

            TEST(Exec, DISABLED_ClassMemberAssign)
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

                const auto result = source_executor_t{facade}.execute(source);
                EXPECT_THAT(result, 2);
            }

            TEST(Exec, DISABLED_ClassMemberFunction)
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

                const auto result = source_executor_t{facade}.execute(source);
                EXPECT_THAT(result, 2);
            }

            TEST(Exec, DISABLED_ClassMemberFunctionUsingClassMember)
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

                const auto result = source_executor_t{facade}.execute(source);
                EXPECT_THAT(result, 5);
            }

            TEST(Exec, DISABLED_UserTypeClassMember)
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

                const auto result = source_executor_t{facade}.execute(source);
                EXPECT_THAT(result, 2);
            }

            TEST(Exec, DISABLED_ListPushBack)
            {
                const auto source =
                    "int main()"
                    "{"
                    "    list<int> l;"
                    "    l.push_back(1);"
                    "    l.push_back(2);"
                    "    l.push_back(3);"
                    "    l.push_back(4);"
                    "    l.push_back(5);"
                    "    return l.size();"
                    "}";

                const auto result = source_executor_t{facade}.execute(source);
                EXPECT_THAT(result, 5);
            }

            TEST(Exec, DISABLED_ListOfListPushBack)
            {
                const auto source =
                    "int main()"
                    "{"
                    "    list<int> l;"
                    "    list<list<int>> ll;"
                    "    ll.push_back(l);"
                    "    ll.push_back(l);"
                    "    ll.push_back(l);"
                    "    ll.push_back(l);"
                    "    ll.push_back(l);"
                    "    return ll.size();"
                    "}";

                const auto result = source_executor_t{facade}.execute(source);
                EXPECT_THAT(result, 5);
            }

            TEST(Exec, DISABLED_UserDefinedConstructor)
            {
                const auto source =
                    "class Foo"
                    "{"
                    "    Foo(int value)"
                    "    {"
                    "        this.m_value = value;"
                    "    }"
                    ""
                    "    int m_value;"
                    "};"
                    "int main()"
                    "{"
                    "    Foo foo = Foo(4);"
                    "    return foo.m_value;"
                    "}";

                const auto result = source_executor_t{facade}.execute(source);
                EXPECT_THAT(result, 4);
            }

            TEST(Exec, DISABLED_VersionCreation)
            {
                const auto source =
                    "int main()"
                    "{"
                    "    version v = version(1, 2, 3, 4);"
                    "    return v.major + v.minor + v.patch + v.tweak;"
                    "}";

                const auto result = source_executor_t{facade}.execute(source);
                EXPECT_THAT(result, 10);
            }
        }
    }
}