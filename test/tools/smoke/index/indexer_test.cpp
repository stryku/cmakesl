#include <gmock/gmock.h>

#include "tools/lib/cmsl_index.hpp"
#include "tools/lib/cmsl_parsed_source.hpp"

namespace cmsl::tools::test {
using ::testing::NotNull;
using ::testing::Eq;

class IndexerSmokeTest : public ::testing::Test
{
protected:
  std::pair<cmsl_parsed_source*, cmsl_index_entries*> index_source(
    const char* source)
  {
    cmsl_parsed_source* parsed_source = cmsl_parse_source(source, nullptr);
    if (parsed_source == nullptr) {
      return {};
    }

    cmsl_index_entries* index_entries = cmsl_index(parsed_source);
    if (index_entries == nullptr) {
      return {};
    }

    return std::make_pair(parsed_source, index_entries);
  }

  void cleanup(cmsl_parsed_source* parsed_source,
               cmsl_index_entries* index_entries)
  {
    cmsl_destroy_index_entries(index_entries);
    cmsl_destroy_parsed_source(parsed_source);
  }
};

TEST_F(IndexerSmokeTest, UnaryOperator)
{
  const auto source = "auto foo = -42;";
  auto [parsed_source, index_entries] = index_source(source);
  ASSERT_THAT(parsed_source, NotNull());
  ASSERT_THAT(index_entries, NotNull());

  // auto and operator
  EXPECT_THAT(index_entries->num_entries, Eq(2u));

  cleanup(parsed_source, index_entries);
}

TEST_F(IndexerSmokeTest, ForLoopInitialization)
{
  const auto source = "void foo()"
                      "{"
                      "    for(int i;;) {}"
                      "}";
  auto [parsed_source, index_entries] = index_source(source);
  ASSERT_THAT(parsed_source, NotNull());
  ASSERT_THAT(index_entries, NotNull());

  // void and int types.
  EXPECT_THAT(index_entries->num_entries, Eq(2u));

  cleanup(parsed_source, index_entries);
}

TEST_F(IndexerSmokeTest, ForLoopCondition)
{
  const auto source = "void foo()"
                      "{"
                      "    int i;"
                      "    for(; i < 0;) {}"
                      "}";
  auto [parsed_source, index_entries] = index_source(source);
  ASSERT_THAT(parsed_source, NotNull());
  ASSERT_THAT(index_entries, NotNull());

  // void and int types, 'i' identifier reference and operator<.
  EXPECT_THAT(index_entries->num_entries, Eq(4u));

  cleanup(parsed_source, index_entries);
}

TEST_F(IndexerSmokeTest, ForLoopIteration)
{
  const auto source = "void foo()"
                      "{"
                      "    int i;"
                      "    for(;;++i) {}"
                      "}";
  auto [parsed_source, index_entries] = index_source(source);
  ASSERT_THAT(parsed_source, NotNull());
  ASSERT_THAT(index_entries, NotNull());

  // void and int types, 'i' identifier reference and operator++.
  EXPECT_THAT(index_entries->num_entries, Eq(4u));

  cleanup(parsed_source, index_entries);
}

TEST_F(IndexerSmokeTest, ForLoopBody)
{
  const auto source = "void foo()"
                      "{"
                      "    int i;"
                      "    for(;;) { ++i; }"
                      "}";
  auto [parsed_source, index_entries] = index_source(source);
  ASSERT_THAT(parsed_source, NotNull());
  ASSERT_THAT(index_entries, NotNull());

  // void and int types, 'i' identifier reference and operator++.
  EXPECT_THAT(index_entries->num_entries, Eq(4u));

  cleanup(parsed_source, index_entries);
}

TEST_F(IndexerSmokeTest, Namespace)
{
  const auto source = "namespace foo"
                      "{"
                      "    int bar;"
                      "}";
  auto [parsed_source, index_entries] = index_source(source);
  ASSERT_THAT(parsed_source, NotNull());
  ASSERT_THAT(index_entries, NotNull());

  // int type
  EXPECT_THAT(index_entries->num_entries, Eq(1u));

  cleanup(parsed_source, index_entries);
}

TEST_F(IndexerSmokeTest, NamespaceWithMultipleNames)
{
  const auto source = "namespace foo::bar::baz"
                      "{"
                      "    int qux;"
                      "}";
  auto [parsed_source, index_entries] = index_source(source);
  ASSERT_THAT(parsed_source, NotNull());
  ASSERT_THAT(index_entries, NotNull());

  // int type
  EXPECT_THAT(index_entries->num_entries, Eq(1u));

  cleanup(parsed_source, index_entries);
}

TEST_F(IndexerSmokeTest, TernaryOperatorCondition)
{
  const auto source = "int foo = 42;"
                      "auto bar = (foo == 42) ? true : false;";
  auto [parsed_source, index_entries] = index_source(source);
  ASSERT_THAT(parsed_source, NotNull());
  ASSERT_THAT(index_entries, NotNull());

  // int, auto, foo identifier and operator==
  EXPECT_THAT(index_entries->num_entries, Eq(4u));

  cleanup(parsed_source, index_entries);
}

TEST_F(IndexerSmokeTest, TernaryOperatorTrueValue)
{
  const auto source = "int foo = 42;"
                      "auto bar = true ? foo : 42;";
  auto [parsed_source, index_entries] = index_source(source);
  ASSERT_THAT(parsed_source, NotNull());
  ASSERT_THAT(index_entries, NotNull());

  // int, auto and foo identifier
  EXPECT_THAT(index_entries->num_entries, Eq(3u));

  cleanup(parsed_source, index_entries);
}

TEST_F(IndexerSmokeTest, TernaryOperatorFalseValue)
{
  const auto source = "int foo = 42;"
                      "auto bar = true ? 42 : foo;";
  auto [parsed_source, index_entries] = index_source(source);
  ASSERT_THAT(parsed_source, NotNull());
  ASSERT_THAT(index_entries, NotNull());

  // int, auto and foo identifier
  EXPECT_THAT(index_entries->num_entries, Eq(3u));

  cleanup(parsed_source, index_entries);
}

TEST_F(IndexerSmokeTest, DesignatedInitializer)
{
  const auto source = "class foo"
                      "{"
                      "    int bar;"
                      "};"
                      ""
                      "int baz = 42;"
                      "foo bar = { .bar = baz };";
  auto [parsed_source, index_entries] = index_source(source);
  ASSERT_THAT(parsed_source, NotNull());
  ASSERT_THAT(index_entries, NotNull());

  // int twice, foo type, bar member, baz identifier
  EXPECT_THAT(index_entries->num_entries, Eq(5u));

  cleanup(parsed_source, index_entries);
}

TEST_F(IndexerSmokeTest, EnumValueConstant)
{
  const auto source = "enum foo"
                      "{"
                      "    bar"
                      "};"
                      ""
                      "auto baz = foo::bar;";
  auto [parsed_source, index_entries] = index_source(source);
  ASSERT_THAT(parsed_source, NotNull());
  ASSERT_THAT(index_entries, NotNull());

  // auto, foo and bar
  EXPECT_THAT(index_entries->num_entries, Eq(3u));

  cleanup(parsed_source, index_entries);
}
}
