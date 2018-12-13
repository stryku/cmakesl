#include "exec/builtin/builtin_function_caller.hpp"
#include "test/exec/mock/cmake_facade_mock.hpp"
#include "test/exec/mock/instances_holder_mock.hpp"
#include "test/exec/mock/instance_mock.hpp"
#include "ast/builtin_ast_context.hpp"
#include "exec/instance/instance_factory.hpp"
#include "exec/instance/instance_value.hpp"

#include <gmock/gmock.h>

namespace cmsl
{
    namespace exec
    {
        namespace builtin
        {
            namespace test
            {
                using ::testing::StrictMock;
                using ::testing::Return;
                using ::testing::Eq;

                // Todo: that's a functional test.
                TEST(BuiltinFunctionCallerTest, VersionConstructor_StoresCorrectValuesInMembers)
                {
                    StrictMock<exec::test::cmake_facade_mock> facade;
                    StrictMock<exec::inst::test::instances_holder_mock> instances;
                    StrictMock<exec::inst::test::instance_mock> major_param_instance;
                    StrictMock<exec::inst::test::instance_mock> minor_param_instance;
                    StrictMock<exec::inst::test::instance_mock> patch_param_instance;
                    StrictMock<exec::inst::test::instance_mock> tweak_param_instance;

                    using int_t = inst::int_t;

                    const auto expected_major = int_t{1};
                    const auto expected_minor = int_t{2};
                    const auto expected_patch = int_t{3};
                    const auto expected_tweak = int_t{4};

                    EXPECT_CALL(major_param_instance, get_value())
                            .WillOnce(Return(inst::instance_value_t{expected_major}));

                    EXPECT_CALL(minor_param_instance, get_value())
                            .WillOnce(Return(inst::instance_value_t{expected_minor}));

                    EXPECT_CALL(patch_param_instance, get_value())
                            .WillOnce(Return(inst::instance_value_t{expected_patch}));

                    EXPECT_CALL(tweak_param_instance, get_value())
                            .WillOnce(Return(inst::instance_value_t{expected_tweak}));

                    std::vector<inst::instance*> parameters{
                            &major_param_instance,
                            &minor_param_instance,
                            &patch_param_instance,
                            &tweak_param_instance
                    };

                    ast::builtin_ast_context ast_ctx;

                    const auto version_type = ast_ctx.find_type("version");
                    auto version_instance = inst::instance_factory{}.create(*version_type);

                    builtin_function_caller caller{instances, facade};

                    auto result = caller.call_member_function(version_instance.get(), "version", parameters);

                    EXPECT_THAT(result, Eq(version_instance.get()));


                    const auto get_int_value = [&version_instance](cmsl::string_view name)
                    {
                        return boost::get<int_t>(version_instance->get_member(name)->get_value());
                    };

                    EXPECT_THAT(get_int_value("major"), Eq(expected_major));
                    EXPECT_THAT(get_int_value("minor"), Eq(expected_minor));
                    EXPECT_THAT(get_int_value("patch"), Eq(expected_patch));
                    EXPECT_THAT(get_int_value("tweak"), Eq(expected_tweak));
                }
            }
        }
    }
}
