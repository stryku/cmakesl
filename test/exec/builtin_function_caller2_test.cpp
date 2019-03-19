#include "exec/builtin_function_caller2.hpp"

#include "test/exec/mock/cmake_facade_mock.hpp"
#include "test/exec/mock/instances_holder_mock.hpp"
#include "test/exec/mock/instance_mock.hpp"

#include <gmock/gmock.h>
#include <sema/builtin_function_kind.hpp>

namespace cmsl
{
    namespace exec
    {
        namespace test
        {
            using testing::StrictMock;
            using testing::Return;
            using testing::ReturnRef;
            using testing::ByMove;
            using testing::_;

            using fun_t = sema::builtin_function_kind;
            using params_t = builtin_function_caller2::params_t;

            // Todo: Consider extracting common CmakeMinimumRequired parts to some function/fixture.
            TEST(BuiltinFunctionCaller2Test, CmakeMinimumRequired_RunningHigherVersion_NoErrorReported)
            {
                StrictMock<exec::test::cmake_facade_mock> facade;
                StrictMock<exec::inst::test::instances_holder_mock> instances;
                StrictMock<exec::inst::test::instance_mock> version_param_instance;
                auto return_instance = std::make_unique<StrictMock<exec::inst::test::instance_mock>>();
                const auto return_instance_ptr = return_instance.get();

                const auto params = params_t{ &version_param_instance };

                const auto test_cmake_version = facade::cmake_facade::version{ 1, 0, 0, 0 };
                EXPECT_CALL(facade, get_cmake_version())
                        .WillOnce(Return(test_cmake_version));

                const auto version_param_value = inst::instance_value_t{ inst::version_value{ 0, 1, 0, 0 } };
                EXPECT_CALL(version_param_instance, get_value_cref())
                        .WillOnce(ReturnRef(version_param_value));

                EXPECT_CALL(instances, create2_void())
                        .WillOnce(Return(return_instance_ptr));

                EXPECT_CALL(instances, gather_ownership(return_instance_ptr))
                        .WillOnce(Return(ByMove(std::move(return_instance))));

                builtin_function_caller2 caller{ facade, instances };
                auto result = caller.call(fun_t::cmake_minimum_required, params);

                EXPECT_THAT(result.get(), Eq(return_instance_ptr));
            }

            TEST(BuiltinFunctionCaller2Test, CmakeMinimumRequired_RunningSameVersion_NoErrorReported)
            {
                StrictMock<exec::test::cmake_facade_mock> facade;
                StrictMock<exec::inst::test::instances_holder_mock> instances;
                StrictMock<exec::inst::test::instance_mock> version_param_instance;
                auto return_instance = std::make_unique<StrictMock<exec::inst::test::instance_mock>>();
                const auto return_instance_ptr = return_instance.get();

                const auto params = params_t{ &version_param_instance };

                const auto test_cmake_version = facade::cmake_facade::version{ 1, 0, 0, 0 };
                EXPECT_CALL(facade, get_cmake_version())
                        .WillOnce(Return(test_cmake_version));

                const auto version_param_value = inst::instance_value_t{ inst::version_value{ 1, 0, 0, 0 } };
                EXPECT_CALL(version_param_instance, get_value_cref())
                        .WillOnce(ReturnRef(version_param_value));

                EXPECT_CALL(instances, create2_void())
                        .WillOnce(Return(return_instance_ptr));

                EXPECT_CALL(instances, gather_ownership(return_instance_ptr))
                        .WillOnce(Return(ByMove(std::move(return_instance))));

                builtin_function_caller2 caller{ facade, instances };
                auto result = caller.call(fun_t::cmake_minimum_required, params);

                EXPECT_THAT(result.get(), Eq(return_instance_ptr));
            }

            TEST(BuiltinFunctionCaller2Test, CmakeMinimumRequired_RunningLowerVersion_FatalErrorReported)
            {
                StrictMock<exec::test::cmake_facade_mock> facade;
                StrictMock<exec::inst::test::instances_holder_mock> instances;
                StrictMock<exec::inst::test::instance_mock> version_param_instance;
                auto return_instance = std::make_unique<StrictMock<exec::inst::test::instance_mock>>();
                const auto return_instance_ptr = return_instance.get();

                const auto params = params_t{ &version_param_instance };

                const auto test_cmake_version = facade::cmake_facade::version{ 1, 0, 0, 0 };
                EXPECT_CALL(facade, get_cmake_version())
                        .WillOnce(Return(test_cmake_version));

                const auto version_param_value = inst::instance_value_t{ inst::version_value{ 2, 0, 0, 0 } };
                EXPECT_CALL(version_param_instance, get_value_cref())
                        .WillOnce(ReturnRef(version_param_value));

                EXPECT_CALL(facade, fatal_error(_));

                EXPECT_CALL(instances, create2_void())
                        .WillOnce(Return(return_instance_ptr));

                EXPECT_CALL(instances, gather_ownership(return_instance_ptr))
                        .WillOnce(Return(ByMove(std::move(return_instance))));

                builtin_function_caller2 caller{ facade, instances };
                auto result = caller.call(fun_t::cmake_minimum_required, params);

                EXPECT_THAT(result.get(), Eq(return_instance_ptr));
            }
        }
    }
}
