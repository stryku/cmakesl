#include "exec/builtin/cmake_minimum_required.hpp"

#include "test/exec/mock/cmake_facade_mock.hpp"

#include <gmock/gmock.h>

namespace cmsl
{
    namespace exec
    {
        namespace builtin
        {
            using testing::StrictMock;
            using testing::Return;

            TEST(CmakeMinimumRequired, RunningHigherVersion_NoErrorReported)
            {
                StrictMock<exec::test::cmake_facade_mock> facade;

                const auto test_cmake_version = facade::cmake_facade::version{ 1, 0, 0, 0 };
                EXPECT_CALL(facade, get_cmake_version())
                        .WillOnce(Return(test_cmake_version));

                const auto test_param_version = facade::cmake_facade::version{ 0, 1, 0, 0 };
                cmake_minimum_required cmr{ facade };
                cmr.call(test_param_version);
            }

            TEST(CmakeMinimumRequired, RunningSameVersion_NoErrorReported)
            {
                StrictMock<exec::test::cmake_facade_mock> facade;
                const auto test_version = facade::cmake_facade::version{ 1, 0, 0, 0 };

                EXPECT_CALL(facade, get_cmake_version())
                        .WillOnce(Return(test_version));

                cmake_minimum_required cmr{ facade };
                cmr.call(test_version);
            }

            TEST(CmakeMinimumRequired, RunningLowerVersion_FatalErrorReported)
            {
                StrictMock<exec::test::cmake_facade_mock> facade;

                const auto test_cmake_version = facade::cmake_facade::version{ 1, 0, 0, 0 };
                EXPECT_CALL(facade, get_cmake_version())
                        .WillOnce(Return(test_cmake_version));

                EXPECT_CALL(facade, fatal_error(_));

                const auto test_param_version = facade::cmake_facade::version{ 2, 0, 0, 0 };
                cmake_minimum_required cmr{ facade };
                cmr.call(test_param_version);
            }
        }
    }
}
