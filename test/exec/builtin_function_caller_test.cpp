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
                using ::testing::NiceMock;
                using ::testing::StrictMock;
                using ::testing::Return;
                using ::testing::ReturnRef;
                using ::testing::ByMove;
                using ::testing::Eq;
                using ::testing::NotNull;
                using ::testing::Const;

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
                        return version_instance->get_member(name)->get_value().get_int();
                    };

                    EXPECT_THAT(get_int_value("major"), Eq(expected_major));
                    EXPECT_THAT(get_int_value("minor"), Eq(expected_minor));
                    EXPECT_THAT(get_int_value("patch"), Eq(expected_patch));
                    EXPECT_THAT(get_int_value("tweak"), Eq(expected_tweak));
                }

                TEST(BuiltinFunctionCallerTest, ProjectConstructor_RegistersProjectInCMakeFacade)
                {
                    StrictMock<exec::test::cmake_facade_mock> facade;
                    StrictMock<exec::inst::test::instances_holder_mock> instances;
                    StrictMock<exec::inst::test::instance_mock> name_param_instance;

                    const auto expected_project_name = std::string{"Project Name"};

                    EXPECT_CALL(name_param_instance, get_value())
                            .WillOnce(Return(inst::instance_value_t{expected_project_name}));

                    EXPECT_CALL(facade, register_project(expected_project_name));

                    ast::builtin_ast_context ast_ctx;
                    const auto project_type = ast_ctx.find_type("project");
                    auto project_instance = inst::instance_factory{}.create(*project_type);

                    std::vector<inst::instance*> parameters{
                            &name_param_instance,
                    };

                    builtin_function_caller caller{instances, facade};
                    auto result = caller.call_member_function(project_instance.get(), "project", parameters);

                    EXPECT_THAT(result, Eq(project_instance.get()));
                }

                TEST(BuiltinFunctionCallerTest, ProjectConstructor_SetsNameMemberValue)
                {
                    NiceMock<exec::test::cmake_facade_mock> facade;
                    StrictMock<exec::inst::test::instances_holder_mock> instances;
                    StrictMock<exec::inst::test::instance_mock> name_param_instance;

                    const auto expected_project_name = std::string{"Project Name"};

                    EXPECT_CALL(name_param_instance, get_value())
                            .WillOnce(Return(inst::instance_value_t{expected_project_name}));

                    ast::builtin_ast_context ast_ctx;
                    const auto project_type = ast_ctx.find_type("project");
                    auto project_instance = inst::instance_factory{}.create(*project_type);

                    std::vector<inst::instance*> parameters{
                            &name_param_instance,
                    };

                    builtin_function_caller caller{instances, facade};
                    auto result = caller.call_member_function(project_instance.get(), "project", parameters);

                    EXPECT_THAT(result, Eq(project_instance.get()));

                    const auto name_member_value = project_instance->get_member("name")->get_value().get_string_cref();
                    EXPECT_THAT(name_member_value, Eq(expected_project_name));
                }

                // Todo: Test crashes because of changes in ast_nodes. Enable it again, when sema nodes will be implemented
                TEST(BuiltinFunctionCallerTest, DISABLED_ProjectAddExecutable_AddsExecutableInCMakeFacade)
                {
                    StrictMock<exec::inst::test::instances_holder_mock> instances;
                    ast::builtin_ast_context ast_ctx;

                    // Setup name parameter.
                    const auto expected_executable_name = std::string{"Executable Name"};
                    inst::instance_value_t name_param_instance_value{expected_executable_name};
                    StrictMock<exec::inst::test::instance_mock> name_param_instance;
                    EXPECT_CALL(name_param_instance, get_value_cref())
                            .WillOnce(ReturnRef(name_param_instance_value));

                    // Setup source list parameter.
                    const auto expected_source_file_name = std::string{"filename.cpp"};
                    auto source_file_instance = std::make_unique<StrictMock<exec::inst::test::instance_mock>>();
                    inst::instance_value_t val{expected_source_file_name};
                    EXPECT_CALL(*source_file_instance, get_value_cref())
                            .WillOnce(ReturnRef(val));

                    const auto& list_type = *ast_ctx.find_type("list<string>");
                    inst::generic_instance_value::list_t sources_list;
                    sources_list.push_back(std::move(source_file_instance));
                    inst::generic_instance_value generic_list_instance_value{ list_type,
                                                                      inst::generic_instance_value::generic_instance_value_type::list,
                                                                      std::move(sources_list) };
                    inst::instance_value_t list_instance_value{std::move(generic_list_instance_value)};

                    StrictMock<exec::inst::test::instance_mock> source_list_param_instance;
                    EXPECT_CALL(Const(source_list_param_instance), get_value_cref())
                            .WillOnce(ReturnRef(list_instance_value));

                    // Setup facade calls.
                    StrictMock<exec::test::cmake_facade_mock> facade;
                    const auto expected_source_list = std::vector<std::string>{
                            expected_source_file_name
                    };
                    EXPECT_CALL(facade, add_executable(expected_executable_name, expected_source_list));

                    // Call the project::add_executable.
                    const auto project_type = ast_ctx.find_type("project");
                    auto project_instance = inst::instance_factory{}.create(*project_type);

                    std::vector<inst::instance*> parameters{
                            &name_param_instance,
                            &source_list_param_instance
                    };

                    builtin_function_caller caller{instances, facade};
                    auto result = caller.call_member_function(project_instance.get(), "add_executable", parameters);

                    EXPECT_THAT(result, NotNull()); // Todo: check if result type is void.
                }
            }
        }
    }
}
