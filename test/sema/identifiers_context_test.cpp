#include "sema/identifiers_context.hpp"
#include "sema/sema_context_impl.hpp"
#include "sema/sema_type.hpp"

#include "test/common/tokens.hpp"

#include <gmock/gmock.h>

namespace cmsl::sema::test {
using ::testing::IsNull;
using ::testing::NotNull;

using namespace cmsl::test::common;

const sema_context_impl valid_context;
const sema_type valid_type{ valid_context,
                            ast::type_representation{
                              token_identifier("foo") },
                            {} };
const sema_type different_type{ valid_context,
                                ast::type_representation{
                                  token_identifier("bar") },
                                {} };

class IdentifiersContextTest : public ::testing::Test
{
  //    protected:
  //        template<typename TokenValue>
  //        auto create_qualified_name(TokenValue &&token_value)
  //        {
  //            const auto token = token_identifier(token_value);
  //            return std::vector<ast::name_with_coloncolon>{{token}};
  //        }

protected:
  template <typename... TokenValues, typename LastTokenValue>
  auto create_qualified_name(LastTokenValue&& last_token_value,
                             TokenValues&&... token_values)
  {
    return std::vector<ast::name_with_coloncolon>{
      (ast::name_with_coloncolon{ token_identifier(token_values),
                                  token_coloncolon() })...,
      ast::name_with_coloncolon{ token_identifier(last_token_value) }
    };
  }
};

TEST_F(IdentifiersContextTest, SingleCtx_NotRegistered_TypeOf_ReturnsNull)
{
  identifiers_context_impl ctx;
  ctx.enter_local_ctx();

  const auto got_info = ctx.info_of(create_qualified_name("foo"));
  EXPECT_THAT(got_info, IsNull());
}

TEST_F(IdentifiersContextTest, MultipleCtxs_NotRegistered_TypeOf_ReturnsNull)
{
  identifiers_context_impl ctx;
  ctx.enter_local_ctx();
  ctx.enter_local_ctx();
  ctx.enter_local_ctx();

  const auto got_info = ctx.info_of(create_qualified_name("foo"));
  EXPECT_THAT(got_info, IsNull());
}

TEST_F(IdentifiersContextTest,
       SingleCtx_TypeOfLocalRegistered_TypeOf_ReturnsCorrectType)
{
  identifiers_context_impl ctx;
  ctx.enter_local_ctx();

  const auto declaration_token = token_identifier("foo");
  const auto identifier_index = 0u;
  ctx.register_identifier(declaration_token, { valid_type, identifier_index });

  const auto got_info = ctx.info_of(create_qualified_name("foo"));
  ASSERT_THAT(got_info, NotNull());
  EXPECT_THAT(&(got_info->type.get()), &valid_type);
  EXPECT_THAT(got_info->index, identifier_index);
}

TEST_F(
  IdentifiersContextTest,
  SingleCtx_LocalRegisteredAmongOtherIdentifiers_TypeOf_ReturnsCorrectType)
{
  identifiers_context_impl ctx;
  ctx.enter_local_ctx();

  const auto declaration_token = token_identifier("foo");
  const auto identifier_index = 0u;
  const auto different_declaration_token = token_identifier("bar");
  const auto different_identifier_index = 1u;

  ctx.register_identifier(declaration_token, { valid_type, identifier_index });
  ctx.register_identifier(different_declaration_token,
                          { different_type, different_identifier_index });

  const auto got_info = ctx.info_of(create_qualified_name("foo"));
  ASSERT_THAT(got_info, NotNull());
  EXPECT_THAT(&(got_info->type.get()), &valid_type);
  EXPECT_THAT(got_info->index, identifier_index);
}

TEST_F(IdentifiersContextTest,
       MultipleCtxs_LocalRegisteredInChildCtx_TypeOf_ReturnsCorrectType)
{
  identifiers_context_impl ctx;
  ctx.enter_local_ctx();
  ctx.enter_local_ctx();

  const auto declaration_token = token_identifier("foo");
  const auto identifier_index = 0u;
  ctx.register_identifier(declaration_token, { valid_type, identifier_index });

  const auto got_info = ctx.info_of(create_qualified_name("foo"));
  ASSERT_THAT(got_info, NotNull());
  EXPECT_THAT(&(got_info->type.get()), &valid_type);
  EXPECT_THAT(got_info->index, identifier_index);
}

TEST_F(IdentifiersContextTest,
       MultipleCtxs_LocalRegisteredInParentCtx_TypeOf_ReturnsCorrectType)
{
  identifiers_context_impl ctx;
  ctx.enter_local_ctx();

  const auto declaration_token = token_identifier("foo");
  const auto identifier_index = 0u;
  ctx.register_identifier(declaration_token, { valid_type, identifier_index });

  ctx.enter_local_ctx();

  const auto got_info = ctx.info_of(create_qualified_name("foo"));
  ASSERT_THAT(got_info, NotNull());
  EXPECT_THAT(&(got_info->type.get()), &valid_type);
  EXPECT_THAT(got_info->index, identifier_index);
}

TEST_F(
  IdentifiersContextTest,
  MultipleCtxs_LocalRegisteredInParentAndChildCtxs_TypeOf_ReturnsTypeFromNestedCtx)
{
  identifiers_context_impl ctx;
  ctx.enter_local_ctx();
  const auto declaration_token = token_identifier("foo");
  const auto identifier_index = 0u;
  ctx.register_identifier(declaration_token, { valid_type, identifier_index });

  ctx.enter_local_ctx();

  const auto different_declaration_token = token_identifier("foo");
  const auto different_identifier_index = 1u;
  ctx.register_identifier(different_declaration_token,
                          { different_type, different_identifier_index });

  const auto got_info = ctx.info_of(create_qualified_name("foo"));
  ASSERT_THAT(got_info, NotNull());
  EXPECT_THAT(&(got_info->type.get()), &different_type);
  EXPECT_THAT(got_info->index, different_identifier_index);
}

TEST_F(
  IdentifiersContextTest,
  MultipleCtxs_LocalRegisteredInParentAndChildCtxs_TypeOfAfterLeavingCtx_ReturnsCorrectType)
{
  identifiers_context_impl ctx;
  ctx.enter_local_ctx();
  const auto declaration_token = token_identifier("foo");
  const auto identifier_index = 0u;
  ctx.register_identifier(declaration_token, { valid_type, identifier_index });

  ctx.enter_local_ctx();
  const auto different_declaration_token = token_identifier("foo");
  const auto different_identifier_index = 0u;
  ctx.register_identifier(different_declaration_token,
                          { different_type, different_identifier_index });

  ctx.leave_ctx();

  const auto got_info = ctx.info_of(create_qualified_name("foo"));
  ASSERT_THAT(got_info, NotNull());
  EXPECT_THAT(&(got_info->type.get()), &valid_type);
  EXPECT_THAT(got_info->index, identifier_index);
}

TEST_F(IdentifiersContextTest,
       WithinGlobalCtxWithNothingRegistered_TypeOf_ReturnsNull)
{
  identifiers_context_impl ctx;
  ctx.enter_global_ctx("foo");

  const auto got_info = ctx.info_of(create_qualified_name("bar"));
  EXPECT_THAT(got_info, IsNull());
}

TEST_F(IdentifiersContextTest,
       WithinGlobalCtxWithNothingRegistered_TypeOfSameNameAsCtx_ReturnsNull)
{
  identifiers_context_impl ctx;
  ctx.enter_global_ctx("foo");

  const auto got_info = ctx.info_of(create_qualified_name("foo"));
  EXPECT_THAT(got_info, IsNull());
}

TEST_F(IdentifiersContextTest,
       AfterLeavingGlobalCtxWithNothingRegistered_TypeOf_ReturnsNull)
{
  identifiers_context_impl ctx;
  ctx.enter_global_ctx("foo");
  ctx.leave_ctx();

  const auto got_info = ctx.info_of(create_qualified_name("foo"));
  EXPECT_THAT(got_info, IsNull());
}

TEST_F(IdentifiersContextTest,
       RootCtx_TypeOfRegistered_TypeOf_ReturnsCorrectType)
{
  identifiers_context_impl ctx;

  const auto declaration_token = token_identifier("foo");
  const auto identifier_index = 0u;
  ctx.register_identifier(declaration_token, { valid_type, identifier_index });

  const auto got_info = ctx.info_of(create_qualified_name("foo"));
  ASSERT_THAT(got_info, NotNull());
  EXPECT_THAT(&(got_info->type.get()), &valid_type);
  EXPECT_THAT(got_info->index, identifier_index);
}

TEST_F(IdentifiersContextTest,
       SingleGlobalCtx_TypeOfRegistered_TypeOf_ReturnsCorrectType)
{
  identifiers_context_impl ctx;
  ctx.enter_global_ctx("ctx");

  const auto declaration_token = token_identifier("foo");
  const auto identifier_index = 0u;
  ctx.register_identifier(declaration_token, { valid_type, identifier_index });

  const auto got_info = ctx.info_of(create_qualified_name("foo"));
  ASSERT_THAT(got_info, NotNull());
  EXPECT_THAT(&(got_info->type.get()), &valid_type);
  EXPECT_THAT(got_info->index, identifier_index);
}

TEST_F(
  IdentifiersContextTest,
  SingleGlobalCtx_RegisteredAmongOtherIdentifiers_TypeOf_ReturnsCorrectType)
{
  identifiers_context_impl ctx;
  ctx.enter_global_ctx("ctx");

  const auto declaration_token = token_identifier("foo");
  const auto identifier_index = 0u;
  const auto different_declaration_token = token_identifier("bar");
  const auto different_identifier_index = 1u;

  ctx.register_identifier(declaration_token, { valid_type, identifier_index });
  ctx.register_identifier(different_declaration_token,
                          { different_type, different_identifier_index });

  const auto got_info = ctx.info_of(create_qualified_name("foo"));
  ASSERT_THAT(got_info, NotNull());
  EXPECT_THAT(&(got_info->type.get()), &valid_type);
  EXPECT_THAT(got_info->index, identifier_index);
}

TEST_F(IdentifiersContextTest,
       MultipleGlobalCtxs_RegisteredInChildCtx_TypeOf_ReturnsCorrectType)
{
  identifiers_context_impl ctx;
  ctx.enter_global_ctx("ctx");
  ctx.enter_global_ctx("other_ctx");

  const auto declaration_token = token_identifier("foo");
  const auto identifier_index = 0u;
  ctx.register_identifier(declaration_token, { valid_type, identifier_index });

  const auto got_info = ctx.info_of(create_qualified_name("foo"));
  ASSERT_THAT(got_info, NotNull());
  EXPECT_THAT(&(got_info->type.get()), &valid_type);
  EXPECT_THAT(got_info->index, identifier_index);
}

TEST_F(IdentifiersContextTest,
       MultipleGlobalCtxs_RegisteredInParentCtx_TypeOf_ReturnsCorrectType)
{
  identifiers_context_impl ctx;
  ctx.enter_global_ctx("ctx");

  const auto declaration_token = token_identifier("foo");
  const auto identifier_index = 0u;
  ctx.register_identifier(declaration_token, { valid_type, identifier_index });

  ctx.enter_global_ctx("other_ctx");

  const auto got_info = ctx.info_of(create_qualified_name("foo"));
  ASSERT_THAT(got_info, NotNull());
  EXPECT_THAT(&(got_info->type.get()), &valid_type);
  EXPECT_THAT(got_info->index, identifier_index);
}

TEST_F(
  IdentifiersContextTest,
  MultipleGlobalCtxs_lRegisteredInParentAndChildCtxs_TypeOf_ReturnsTypeFromNestedCtx)
{
  identifiers_context_impl ctx;
  ctx.enter_global_ctx("ctx");
  const auto declaration_token = token_identifier("foo");
  const auto identifier_index = 0u;
  ctx.register_identifier(declaration_token, { valid_type, identifier_index });

  ctx.enter_global_ctx("other_ctx");

  const auto different_declaration_token = token_identifier("foo");
  const auto different_identifier_index = 1u;
  ctx.register_identifier(different_declaration_token,
                          { different_type, different_identifier_index });

  const auto got_info = ctx.info_of(create_qualified_name("foo"));
  ASSERT_THAT(got_info, NotNull());
  EXPECT_THAT(&(got_info->type.get()), &different_type);
  EXPECT_THAT(got_info->index, different_identifier_index);
}

TEST_F(
  IdentifiersContextTest,
  MultipleGlobal_RegisteredInParentAndChildCtxs_TypeOfAfterLeavingCtx_ReturnsFromParentCtx)
{
  identifiers_context_impl ctx;
  ctx.enter_global_ctx("ctx");
  const auto declaration_token = token_identifier("foo");
  const auto identifier_index = 0u;
  ctx.register_identifier(declaration_token, { valid_type, identifier_index });

  ctx.enter_global_ctx("other_ctx");

  const auto different_declaration_token = token_identifier("foo");
  const auto different_identifier_index = 1u;
  ctx.register_identifier(different_declaration_token,
                          { different_type, different_identifier_index });

  ctx.leave_ctx();

  const auto got_info = ctx.info_of(create_qualified_name("foo"));
  ASSERT_THAT(got_info, NotNull());
  EXPECT_THAT(&(got_info->type.get()), &valid_type);
  EXPECT_THAT(got_info->index, identifier_index);
}

TEST_F(IdentifiersContextTest,
       GlobalAndLocalCtxs_RegisteredInGlobalCtx_TypeOf_ReturnsFromGlobalCtx)
{
  identifiers_context_impl ctx;

  const auto declaration_token = token_identifier("foo");
  const auto identifier_index = 0u;
  ctx.register_identifier(declaration_token, { valid_type, identifier_index });

  ctx.enter_local_ctx();

  const auto got_info = ctx.info_of(create_qualified_name("foo"));
  ASSERT_THAT(got_info, NotNull());
  EXPECT_THAT(&(got_info->type.get()), &valid_type);
  EXPECT_THAT(got_info->index, identifier_index);
}

TEST_F(IdentifiersContextTest,
       GlobalAndLocalCtxs_RegisteredInLocalCtx_TypeOf_ReturnsFromLocalCtx)
{
  identifiers_context_impl ctx;

  ctx.enter_local_ctx();

  const auto declaration_token = token_identifier("foo");
  const auto identifier_index = 0u;
  ctx.register_identifier(declaration_token, { valid_type, identifier_index });

  const auto got_info = ctx.info_of(create_qualified_name("foo"));
  ASSERT_THAT(got_info, NotNull());
  EXPECT_THAT(&(got_info->type.get()), &valid_type);
  EXPECT_THAT(got_info->index, identifier_index);
}

TEST_F(
  IdentifiersContextTest,
  GlobalAndLocalCtxs_RegisteredInLocalAndGlobalCtx_TypeOf_ReturnsFromLocalCtx)
{
  identifiers_context_impl ctx;
  ctx.enter_global_ctx("ctx");

  const auto declaration_token = token_identifier("foo");
  const auto identifier_index = 0u;
  ctx.register_identifier(declaration_token, { valid_type, identifier_index });

  ctx.enter_local_ctx();

  const auto different_declaration_token = token_identifier("foo");
  const auto different_identifier_index = 1u;
  ctx.register_identifier(different_declaration_token,
                          { different_type, different_identifier_index });

  const auto got_info = ctx.info_of(create_qualified_name("foo"));
  ASSERT_THAT(got_info, NotNull());
  EXPECT_THAT(&(got_info->type.get()), &different_type);
  EXPECT_THAT(got_info->index, different_identifier_index);
}

TEST_F(IdentifiersContextTest,
       GlobalCtxs_RegisteredInTopGlobalCtx_TypeOf_ReturnsFromTopGlobalCtx)
{
  identifiers_context_impl ctx;

  const auto declaration_token = token_identifier("foo");
  const auto identifier_index = 0u;
  ctx.register_identifier(declaration_token, { valid_type, identifier_index });

  ctx.enter_global_ctx("ctx");

  const auto different_declaration_token = token_identifier("foo");
  const auto different_identifier_index = 1u;
  ctx.register_identifier(different_declaration_token,
                          { different_type, different_identifier_index });

  // ::foo
  const auto got_info = ctx.info_of(create_qualified_name("foo", ""));
  ASSERT_THAT(got_info, NotNull());
  EXPECT_THAT(&(got_info->type.get()), &valid_type);
  EXPECT_THAT(got_info->index, identifier_index);
}

TEST_F(
  IdentifiersContextTest,
  GlobalCtxs_RegisteredInTopAndNestedGlobalCtx_TypeOf_ReturnsFromNestedGlobalCtx)
{
  identifiers_context_impl ctx;

  const auto declaration_token = token_identifier("foo");
  const auto identifier_index = 0u;
  ctx.register_identifier(declaration_token, { valid_type, identifier_index });

  ctx.enter_global_ctx("ctx");

  const auto different_declaration_token = token_identifier("foo");
  const auto different_identifier_index = 1u;
  ctx.register_identifier(different_declaration_token,
                          { different_type, different_identifier_index });

  {
    // ctx::foo
    const auto got_info = ctx.info_of(create_qualified_name("foo", "ctx"));
    ASSERT_THAT(got_info, NotNull());
    EXPECT_THAT(&(got_info->type.get()), &different_type);
    EXPECT_THAT(got_info->index, different_identifier_index);
  }
  {

    // ::ctx::foo
    const auto got_info = ctx.info_of(create_qualified_name("foo", "", "ctx"));
    ASSERT_THAT(got_info, NotNull());
    EXPECT_THAT(&(got_info->type.get()), &different_type);
    EXPECT_THAT(got_info->index, different_identifier_index);
  }
}

TEST_F(IdentifiersContextTest,
       GlobalCtxs_RegisteredInGlobalAndLocalCtxs_TypeOf_ReturnsFromGlobalCtx)
{
  identifiers_context_impl ctx;

  const auto declaration_token = token_identifier("foo");
  const auto identifier_index = 0u;
  ctx.register_identifier(declaration_token, { valid_type, identifier_index });

  ctx.enter_global_ctx("ctx");

  const auto different_declaration_token = token_identifier("foo");
  const auto different_identifier_index = 1u;
  ctx.register_identifier(different_declaration_token,
                          { different_type, different_identifier_index });

  ctx.enter_local_ctx();

  const auto third_declaration_token = token_identifier("foo");
  const auto third_identifier_index = 1u;
  ctx.register_identifier(third_declaration_token,
                          { different_type, third_identifier_index });

  // ::ctx::foo
  const auto got_info = ctx.info_of(create_qualified_name("foo", "", "ctx"));
  ASSERT_THAT(got_info, NotNull());
  EXPECT_THAT(&(got_info->type.get()), &different_type);
  EXPECT_THAT(got_info->index, different_identifier_index);
}
}
