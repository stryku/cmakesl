#include "sema/identifiers_context.hpp"
#include "sema/sema_context_impl.hpp"
#include "sema/sema_type.hpp"

#include "test/common/tokens.hpp"

#include <gmock/gmock.h>

namespace cmsl::sema::test {
using ::testing::IsNull;

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

TEST(IdentifiersContextTest, SingleScope_NotRegistered_TypeOf_ReturnsNull)
{
  identifiers_context_impl ctx;
  ctx.enter_ctx();

  const auto got_type = ctx.type_of("foo");

  EXPECT_THAT(got_type, IsNull());
}

TEST(IdentifiersContextTest, MultipleScopes_NotRegistered_TypeOf_ReturnsNull)
{
  identifiers_context_impl ctx;
  ctx.enter_ctx();
  ctx.enter_ctx();
  ctx.enter_ctx();

  const auto got_type = ctx.type_of("foo");

  EXPECT_THAT(got_type, IsNull());
}

TEST(IdentifiersContextTest,
     SingleScope_TypeOfRegistered_TypeOf_ReturnsCorrectType)
{
  identifiers_context_impl ctx;
  ctx.enter_ctx();

  const auto declaration_token = token_identifier("foo");
  ctx.register_identifier(declaration_token, valid_type);

  const auto got_type = ctx.type_of("foo");

  EXPECT_THAT(got_type, &valid_type);
}

TEST(IdentifiersContextTest,
     SingleScope_RegisteredAmongOtherIdentifiers_TypeOf_ReturnsCorrectType)
{
  identifiers_context_impl ctx;
  ctx.enter_ctx();

  const auto declaration_token = token_identifier("foo");
  const auto different_declaration_token = token_identifier("bar");

  ctx.register_identifier(declaration_token, valid_type);
  ctx.register_identifier(different_declaration_token, different_type);

  const auto got_type = ctx.type_of("foo");

  EXPECT_THAT(got_type, &valid_type);
}

TEST(IdentifiersContextTest,
     MultipleScopes_RegisteredInChildScope_TypeOf_ReturnsCorrectType)
{
  identifiers_context_impl ctx;
  ctx.enter_ctx();
  ctx.enter_ctx();

  const auto declaration_token = token_identifier("foo");
  ctx.register_identifier(declaration_token, valid_type);

  const auto got_type = ctx.type_of("foo");

  EXPECT_THAT(got_type, &valid_type);
}

TEST(IdentifiersContextTest,
     MultipleScopes_RegisteredInParentScope_TypeOf_ReturnsCorrectType)
{
  identifiers_context_impl ctx;
  ctx.enter_ctx();

  const auto declaration_token = token_identifier("foo");
  ctx.register_identifier(declaration_token, valid_type);

  ctx.enter_ctx();

  const auto got_type = ctx.type_of("foo");

  EXPECT_THAT(got_type, &valid_type);
}

TEST(IdentifiersContextTest,
     MultipleScopes_RegisteredInParentAndChildScopes_TypeOf_ReturnsCorrectType)
{
  identifiers_context_impl ctx;
  ctx.enter_ctx();
  const auto declaration_token = token_identifier("foo");
  ctx.register_identifier(declaration_token, valid_type);

  ctx.enter_ctx();

  const auto different_declaration_token = token_identifier("foo");
  ctx.register_identifier(different_declaration_token, different_type);

  const auto got_type = ctx.type_of("foo");

  EXPECT_THAT(got_type, &different_type);
}

TEST(
  IdentifiersContextTest,
  MultipleScopes_RegisteredInParentAndChildScopes_TypeOfAfterLeavingScope_ReturnsCorrectType)
{
  identifiers_context_impl ctx;
  ctx.enter_ctx();
  const auto declaration_token = token_identifier("foo");
  ctx.register_identifier(declaration_token, valid_type);

  ctx.enter_ctx();
  const auto different_declaration_token = token_identifier("foo");
  ctx.register_identifier(different_declaration_token, different_type);

  ctx.leave_ctx();

  const auto got_type = ctx.type_of("foo");

  EXPECT_THAT(got_type, &valid_type);
}
}
