#include "exec/builtin/cmake_minimum_required.hpp"

#include "ast/type.hpp"

#include "test/exec/mock/cmake_facade_mock.hpp"
#include "test/exec/mock/instances_holder_mock.hpp"
#include "test/exec/mock/instance_mock.hpp"

#include <gmock/gmock.h>

namespace cmsl
{
    namespace exec
    {
        namespace builtin
        {
            namespace test
            {
                using testing::StrictMock;
                using testing::Return;
                using testing::_;

                TEST(CmakeMinimumRequired, RunningHigherVersion_NoErrorReported)
                {
                    StrictMock<exec::test::cmake_facade_mock> facade;
                    StrictMock<exec::inst::test::instances_holder_mock> instances;
                    StrictMock<exec::inst::test::instance_mock> return_instance;

                    const auto test_cmake_version = facade::cmake_facade::version{ 1, 0, 0, 0 };
                    EXPECT_CALL(facade, get_cmake_version())
                            .WillOnce(Return(test_cmake_version));

                    EXPECT_CALL(instances, create_void())
                                .WillOnce(Return(&return_instance));

                    const auto test_param_version = facade::cmake_facade::version{ 0, 1, 0, 0 };
                    cmake_minimum_required cmr{ facade, instances, test_param_version };
                    auto result = cmr.evaluate();

                    EXPECT_THAT(result, Eq(&return_instance));
                }

                TEST(CmakeMinimumRequired, RunningSameVersion_NoErrorReported)
                {
                    StrictMock<exec::test::cmake_facade_mock> facade;
                    StrictMock<exec::inst::test::instances_holder_mock> instances;
                    StrictMock<exec::inst::test::instance_mock> return_instance;
                    const auto test_version = facade::cmake_facade::version{ 1, 0, 0, 0 };

                    EXPECT_CALL(facade, get_cmake_version())
                            .WillOnce(Return(test_version));

                    EXPECT_CALL(instances, create_void())
                            .WillOnce(Return(&return_instance));

                    cmake_minimum_required cmr{ facade, instances, test_version };
                    auto result = cmr.evaluate();

                    EXPECT_THAT(result, Eq(&return_instance));
                }

                TEST(CmakeMinimumRequired, RunningLowerVersion_FatalErrorReported)
                {
                    StrictMock<exec::test::cmake_facade_mock> facade;
                    StrictMock<exec::inst::test::instances_holder_mock> instances;
                    StrictMock<exec::inst::test::instance_mock> return_instance;

                    const auto test_cmake_version = facade::cmake_facade::version{ 1, 0, 0, 0 };
                    EXPECT_CALL(facade, get_cmake_version())
                            .WillOnce(Return(test_cmake_version));

                    EXPECT_CALL(facade, fatal_error(_));

                    EXPECT_CALL(instances, create_void())
                            .WillOnce(Return(&return_instance));

                    const auto test_param_version = facade::cmake_facade::version{ 2, 0, 0, 0 };
                    cmake_minimum_required cmr{ facade, instances, test_param_version };
                    auto result = cmr.evaluate();

                    EXPECT_THAT(result, Eq(&return_instance));
                }
            }
        }
    }
}
