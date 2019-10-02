#include "exec/builtin_function_caller.hpp"

#include "sema/builtin_types_accessor.hpp"
#include "sema/sema_context_impl.hpp"
#include "sema/sema_type.hpp"

#include "test/common/tokens.hpp"
#include "test/exec/mock/instance_mock.hpp"
#include "test/exec/mock/instances_holder_mock.hpp"
#include "test/mock/cmake_facade_mock.hpp"

#include <gmock/gmock.h>
#include <sema/builtin_function_kind.hpp>

namespace cmsl::exec::test {
using testing::StrictMock;
using testing::Return;
using testing::ReturnRef;
using testing::ByMove;
using testing::_;

using fun_t = sema::builtin_function_kind;
using params_t = builtin_function_caller::params_t;

using namespace cmsl::test::common;

const sema::sema_context_impl builtin_context_fake{ "" };

struct type_data
{
  ast::type_representation representation;
  sema::sema_context_impl ctx;
  sema::sema_type ty{ ctx, representation, {} };
  std::vector<ast::name_with_coloncolon> qualified_names =
    representation.qual_name().names();
};

type_data create_type_data(lexer::token token)
{
  return type_data{ ast::type_representation{ ast::qualified_name{ token } },
                    sema::sema_context_impl{ std::string{ token.str() },
                                             &builtin_context_fake } };
}

type_data valid_type_data = create_type_data(token_identifier("foo"));
const sema::sema_type valid_type_reference{ sema::sema_type_reference{
  valid_type_data.ty } };
type_data different_type_data =
  create_type_data(token_identifier("different_type"));

class BuiltinFunctionCallerTest : public ::testing::Test
{
protected:
  type_data m_void_type_data = create_type_data(token_kw_void());
  type_data m_bool_type_data = create_type_data(token_kw_bool());
  type_data m_int_type_data = create_type_data(token_kw_int());
  type_data m_double_type_data = create_type_data(token_kw_double());
  type_data m_string_type_data = create_type_data(token_kw_string());
  type_data m_version_type_data = create_type_data(token_kw_version());
  type_data m_library_type_data = create_type_data(token_kw_library());
  type_data m_executable_type_data = create_type_data(token_kw_executable());
  type_data m_project_type_data = create_type_data(token_kw_project());

  const sema::builtin_types_accessor m_builtin_types =
    sema::builtin_types_accessor{ .void_ = m_void_type_data.ty,
                                  .bool_ = m_bool_type_data.ty,
                                  .bool_ref = valid_type_data.ty,
                                  .int_ = m_int_type_data.ty,
                                  .int_ref = valid_type_data.ty,
                                  .double_ = m_double_type_data.ty,
                                  .double_ref = valid_type_data.ty,
                                  .string = m_string_type_data.ty,
                                  .string_ref = valid_type_data.ty };
};

// Todo: Consider extracting common CmakeMinimumRequired parts to some
// function/fixture.
TEST_F(BuiltinFunctionCallerTest,
       CmakeMinimumRequired_RunningHigherVersion_NoErrorReported)
{
  StrictMock<exec::test::cmake_facade_mock> facade;
  StrictMock<exec::inst::test::instances_holder_mock> instances;
  StrictMock<exec::inst::test::instance_mock> version_param_instance;
  auto return_instance =
    std::make_unique<StrictMock<exec::inst::test::instance_mock>>();
  const auto return_instance_ptr = return_instance.get();

  const auto params = params_t{ &version_param_instance };

  const auto test_cmake_version = facade::cmake_facade::version{ 1, 0, 0, 0 };
  EXPECT_CALL(facade, get_cmake_version())
    .WillOnce(Return(test_cmake_version));

  const auto version_param_value =
    inst::instance_value_variant{ inst::version_value{ 0, 1, 0, 0 } };
  EXPECT_CALL(version_param_instance, value_cref())
    .WillOnce(ReturnRef(version_param_value));

  EXPECT_CALL(instances, create_void()).WillOnce(Return(return_instance_ptr));

  EXPECT_CALL(instances, gather_ownership(return_instance_ptr))
    .WillOnce(Return(ByMove(std::move(return_instance))));

  builtin_function_caller caller{ facade, instances, m_builtin_types };
  auto result = caller.call(fun_t::cmake_minimum_required, params);

  EXPECT_THAT(result.get(), Eq(return_instance_ptr));
}

TEST_F(BuiltinFunctionCallerTest,
       CmakeMinimumRequired_RunningSameVersion_NoErrorReported)
{
  StrictMock<exec::test::cmake_facade_mock> facade;
  StrictMock<exec::inst::test::instances_holder_mock> instances;
  StrictMock<exec::inst::test::instance_mock> version_param_instance;
  auto return_instance =
    std::make_unique<StrictMock<exec::inst::test::instance_mock>>();
  const auto return_instance_ptr = return_instance.get();

  const auto params = params_t{ &version_param_instance };

  const auto test_cmake_version = facade::cmake_facade::version{ 1, 0, 0, 0 };
  EXPECT_CALL(facade, get_cmake_version())
    .WillOnce(Return(test_cmake_version));

  const auto version_param_value =
    inst::instance_value_variant{ inst::version_value{ 1, 0, 0, 0 } };
  EXPECT_CALL(version_param_instance, value_cref())
    .WillOnce(ReturnRef(version_param_value));

  EXPECT_CALL(instances, create_void()).WillOnce(Return(return_instance_ptr));

  EXPECT_CALL(instances, gather_ownership(return_instance_ptr))
    .WillOnce(Return(ByMove(std::move(return_instance))));

  builtin_function_caller caller{ facade, instances, m_builtin_types };
  auto result = caller.call(fun_t::cmake_minimum_required, params);

  EXPECT_THAT(result.get(), Eq(return_instance_ptr));
}

TEST_F(BuiltinFunctionCallerTest,
       CmakeMinimumRequired_RunningLowerVersion_FatalErrorReported)
{
  StrictMock<exec::test::cmake_facade_mock> facade;
  StrictMock<exec::inst::test::instances_holder_mock> instances;
  StrictMock<exec::inst::test::instance_mock> version_param_instance;
  auto return_instance =
    std::make_unique<StrictMock<exec::inst::test::instance_mock>>();
  const auto return_instance_ptr = return_instance.get();

  const auto params = params_t{ &version_param_instance };

  const auto test_cmake_version = facade::cmake_facade::version{ 1, 0, 0, 0 };
  EXPECT_CALL(facade, get_cmake_version())
    .WillOnce(Return(test_cmake_version));

  const auto version_param_value =
    inst::instance_value_variant{ inst::version_value{ 2, 0, 0, 0 } };
  EXPECT_CALL(version_param_instance, value_cref())
    .WillOnce(ReturnRef(version_param_value));

  EXPECT_CALL(facade, fatal_error(_));

  EXPECT_CALL(instances, create_void()).WillOnce(Return(return_instance_ptr));

  EXPECT_CALL(instances, gather_ownership(return_instance_ptr))
    .WillOnce(Return(ByMove(std::move(return_instance))));

  builtin_function_caller caller{ facade, instances, m_builtin_types };
  auto result = caller.call(fun_t::cmake_minimum_required, params);

  EXPECT_THAT(result.get(), Eq(return_instance_ptr));
}
}
