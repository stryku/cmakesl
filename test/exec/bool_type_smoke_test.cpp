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

            class BoolTypeSmokeTest : public ::testing::Test
            {
            protected:
                cmake_facade_mock m_facade;
                source_executor m_executor{m_facade};
            };

            TEST_F(BoolTypeSmokeTest, DefaultConstructor)
            {
                const auto source =
                        "int main()\n"
                        "{\n"
                        "    bool b;\n"
                        "    return int(b);\n"
                        "}";
                const auto result = m_executor.execute2(source);
                EXPECT_THAT(result, Eq(0));
            }
        }
    }
}
