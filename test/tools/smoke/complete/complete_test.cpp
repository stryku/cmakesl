#include <gmock/gmock.h>

#include "tools/lib/cmsl_complete.hpp"
#include "tools/lib/cmsl_parsed_source.hpp"

namespace {
template <typename Container>
auto sort(Container c)
{
  std::sort(std::begin(c), std::end(c));
  return c;
}

template <typename... Ts>
auto merge(Ts&&... values)
{
  std::vector<std::string> result;

  const auto inserter = [&result](const auto& value) {
    result.insert(std::end(result), std::cbegin(value), std::cend(value));
  };

  (inserter(values), ...);

  return result;
}

auto sort_results(cmsl_complete_results* results)
{
  std::vector<std::string> v;
  v.reserve(results->num_results);

  for (auto i = 0u; i < results->num_results; ++i) {
    v.emplace_back(results->results[i]);
  }

  return sort(std::move(v));
}

}

namespace cmsl::tools::test {
using ::testing::NotNull;
using ::testing::Eq;

class CompleteSmokeTest : public ::testing::Test
{
protected:
  std::pair<cmsl_parsed_source*, cmsl_complete_results*> complete_at(
    const char* source, unsigned position)
  {
    cmsl_parsed_source* parsed_source = cmsl_parse_source(source, nullptr);
    if (parsed_source == nullptr) {
      return {};
    }
    cmsl_complete_results* results = cmsl_complete_at(parsed_source, position);
    if (results == nullptr) {
      return {};
    }

    return std::make_pair(parsed_source, results);
  }

  void cleanup(cmsl_parsed_source* parsed_source,
               cmsl_complete_results* results)
  {
    cmsl_destroy_complete_results(results);
    cmsl_destroy_parsed_source(parsed_source);
  }

  std::vector<std::string> builtin_types() const
  {
    return { "void", "double", "bool", "int", "string", "list<string>" };
  }

  std::vector<std::string> standalone_statement() const
  {
    return { "if", "for", "while" };
  }

  std::vector<std::string> top_level_default_values() const
  {
    auto v = builtin_types();
    v.emplace_back("class");
    return v;
  }
};

TEST_F(CompleteSmokeTest, TopLevelEmpty_ReturnsTopLevelDefaultValues)
{
  const auto source = "";
  auto [parsed_source, results] = complete_at(source, 0u);
  ASSERT_THAT(parsed_source, NotNull());
  ASSERT_THAT(results, NotNull());

  const auto sorted_expected = sort(top_level_default_values());
  const auto sorted_results = sort_results(results);

  EXPECT_THAT(sorted_results, Eq(sorted_expected));

  cleanup(parsed_source, results);
}

TEST_F(CompleteSmokeTest,
       TopLevelWithVariable_CompleteAfterVariable_ReturnsTopLevelDefaultValues)
{
  const auto source = "int foo;";
  auto [parsed_source, results] = complete_at(source, 8u);
  ASSERT_THAT(parsed_source, NotNull());
  ASSERT_THAT(results, NotNull());

  const auto sorted_expected = sort(top_level_default_values());
  const auto sorted_results = sort_results(results);

  EXPECT_THAT(sorted_results, Eq(sorted_expected));

  cleanup(parsed_source, results);
}

TEST_F(
  CompleteSmokeTest,
  TopLevelWithFunction_CompleteInsideFunction_ReturnsStandAloneDefaultValuesAndFunctionName)
{
  const auto source = "int foo() { }";
  auto [parsed_source, results] = complete_at(source, 11u);
  ASSERT_THAT(parsed_source, NotNull());
  ASSERT_THAT(results, NotNull());

  auto expected = merge(builtin_types(), standalone_statement());
  expected.emplace_back("foo");
  const auto sorted_expected = sort(expected);
  const auto sorted_results = sort_results(results);

  EXPECT_THAT(sorted_results, Eq(sorted_expected));

  cleanup(parsed_source, results);
}

TEST_F(
  CompleteSmokeTest,
  TopLevelWithFunctionWithParameter_CompleteInsideFunction_ReturnsStandAloneDefaultValuesAndFunctionNameAndParameterName)
{
  const auto source = "int foo(int bar) { }";
  auto [parsed_source, results] = complete_at(source, 19u);
  ASSERT_THAT(parsed_source, NotNull());
  ASSERT_THAT(results, NotNull());

  auto expected = merge(builtin_types(), standalone_statement());
  expected.emplace_back("foo");
  expected.emplace_back("bar");
  const auto sorted_expected = sort(expected);
  const auto sorted_results = sort_results(results);

  EXPECT_THAT(sorted_results, Eq(sorted_expected));

  cleanup(parsed_source, results);
}

TEST_F(
  CompleteSmokeTest,
  TopLevelWithFunctions_CompleteInsideFunction_ReturnsStandAloneDefaultValuesAndFunctionNames)
{
  const auto source = "int foo() {} int bar() { }";
  auto [parsed_source, results] = complete_at(source, 24u);
  ASSERT_THAT(parsed_source, NotNull());
  ASSERT_THAT(results, NotNull());

  auto expected = merge(builtin_types(), standalone_statement());
  expected.emplace_back("foo");
  expected.emplace_back("bar");
  const auto sorted_expected = sort(expected);
  const auto sorted_results = sort_results(results);

  EXPECT_THAT(sorted_results, Eq(sorted_expected));

  cleanup(parsed_source, results);
}

TEST_F(
  CompleteSmokeTest,
  TopLevelWithClassAndFunction_CompleteInsideFunction_ReturnsStandAloneDefaultValuesAndClassNameAndFunctionName)
{
  const auto source = "class foo{}; int bar() { }";
  auto [parsed_source, results] = complete_at(source, 24u);
  ASSERT_THAT(parsed_source, NotNull());
  ASSERT_THAT(results, NotNull());

  auto expected = merge(builtin_types(), standalone_statement());
  expected.emplace_back("foo");
  expected.emplace_back("bar");
  const auto sorted_expected = sort(expected);
  const auto sorted_results = sort_results(results);

  EXPECT_THAT(sorted_results, Eq(sorted_expected));

  cleanup(parsed_source, results);
}

TEST_F(CompleteSmokeTest,
       TopLevelWithClass_CompleteInsideClass_ReturnsBuiltinTypesAndClassName)
{
  const auto source = "class foo{};";
  auto [parsed_source, results] = complete_at(source, 10u);
  ASSERT_THAT(parsed_source, NotNull());
  ASSERT_THAT(results, NotNull());

  auto expected = builtin_types();
  expected.emplace_back("foo");
  const auto sorted_expected = sort(expected);
  const auto sorted_results = sort_results(results);

  EXPECT_THAT(sorted_results, Eq(sorted_expected));

  cleanup(parsed_source, results);
}

TEST_F(
  CompleteSmokeTest,
  TopLevelWithClassWithMember_CompleteInsideClass_ReturnsBuiltinTypesAndClassName)
{
  const auto source = "class foo{ int bar; };";
  auto [parsed_source, results] = complete_at(source, 10u);
  ASSERT_THAT(parsed_source, NotNull());
  ASSERT_THAT(results, NotNull());

  auto expected = builtin_types();
  expected.emplace_back("foo");
  const auto sorted_expected = sort(expected);
  const auto sorted_results = sort_results(results);

  EXPECT_THAT(sorted_results, Eq(sorted_expected));

  cleanup(parsed_source, results);
}

// Todo: Enable when adding class name in member functions is implemented.
TEST_F(
  CompleteSmokeTest,
  DISABLED_TopLevelWithClassWithMemberAndMethod_CompleteInsideMethod_ReturnsStandAloneDefaultValuesAndClassNameAndMemberNameAndMethodName)
{
  const auto source = "class foo{ int bar; void baz() {} };";
  auto [parsed_source, results] = complete_at(source, 32u);
  ASSERT_THAT(parsed_source, NotNull());
  ASSERT_THAT(results, NotNull());

  auto expected = merge(builtin_types(), standalone_statement());
  expected.emplace_back("foo");
  expected.emplace_back("bar");
  expected.emplace_back("baz");
  const auto sorted_expected = sort(expected);
  const auto sorted_results = sort_results(results);

  EXPECT_THAT(sorted_results, Eq(sorted_expected));

  cleanup(parsed_source, results);
}
}
