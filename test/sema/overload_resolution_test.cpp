#include "sema/overload_resolution.hpp"

#include "errors/errors_observer.hpp"

#include "sema/sema_nodes.hpp"
#include "sema/sema_context.hpp"

#include "test/ast/mock/ast_node_mock.hpp"
#include "test/errors_observer_mock/errors_observer_mock.hpp"
#include "test/common/tokens.hpp"
#include "test/sema/mock/expression_node_mock.hpp"
#include "test/sema/mock/sema_function_mock.hpp"

#include <gmock/gmock.h>

namespace cmsl::sema::test
{
    using ::testing::NiceMock;
    using ::testing::StrictMock;
            using ::testing::Return;
            using ::testing::ReturnRef;
            using ::testing::IsNull;
            using ::testing::NotNull;
            using ::testing::Eq;
            using ::testing::_;

            using namespace cmsl::test::common;

            // Todo: to a common file
            struct errors_observer_and_mock
            {
                StrictMock<errors::test::errors_observer_mock> mock;
                errors::errors_observer observer;
            };
            using errs_t = errors_observer_and_mock;

            const sema_context valid_context;
            const sema_type valid_type{ valid_context, ast::type_representation{ token_identifier("foo") }, {} };


            class OverloarResolutionTest : public ::testing::Test
            {
            public:
                auto expression_mock()
                {
                    static auto ast_node = NiceMock<ast::test::ast_node_mock>{};
                    return std::make_unique<StrictMock<expression_node_mock>>(ast_node);
                }
            };

            // Todo: Consider moving common parts to a fixture.

            TEST_F(OverloarResolutionTest, EmptyFunctionLookupResult_RaiseErrorAndReturnNull)
            {
                errs_t errs;
                const auto call_token = token_identifier("foo");
                function_lookup_result_t lookup_result;

                EXPECT_CALL(errs.mock, notify_error(_));

                overload_resolution resolution{ errs.observer, call_token };
                const auto chosen = resolution.choose(lookup_result, {});

                EXPECT_THAT(chosen, IsNull());
            }

            TEST_F(OverloarResolutionTest, ParamsCountDoesntMatch_RaiseErrorAndReturnNull)
            {
                errs_t errs;
                const auto call_token = token_identifier("foo");

                StrictMock<sema_function_mock> function;

                single_scope_function_lookup_result_t scope_result{ &function };
                function_lookup_result_t lookup_result{ scope_result };

                const auto function_name_token = token_identifier("foo");
                const auto param_name_token = token_identifier("param");
                function_signature signature{
                        function_name_token,
                        { parameter_declaration{ valid_type, param_name_token } }
                };
                EXPECT_CALL(function, signature())
                        .WillOnce(ReturnRef(signature));

                EXPECT_CALL(errs.mock, notify_error(_));

                overload_resolution resolution{ errs.observer, call_token };
                const auto chosen = resolution.choose(lookup_result, {});

                EXPECT_THAT(chosen, IsNull());
            }

            TEST_F(OverloarResolutionTest, ParamTypesDontMatch_RaiseErrorAndReturnNull)
            {
                errs_t errs;
                const auto call_token = token_identifier("foo");
                StrictMock<sema_function_mock> function;
                auto param_expression = expression_mock();
                auto param_expression_ptr = param_expression.get();
                const sema_type param_type{ valid_context, ast::type_representation{ token_identifier("param_type") }, {} };
                std::vector<std::unique_ptr<expression_node>> param_expressions;
                param_expressions.emplace_back(std::move(param_expression));

                single_scope_function_lookup_result_t scope_result{ &function };
                function_lookup_result_t lookup_result{ scope_result };

                const auto function_name_token = token_identifier("foo");
                const auto param_name_token = token_identifier("param");
                function_signature signature{
                        function_name_token,
                        { parameter_declaration{ valid_type, param_name_token } }
                };
                EXPECT_CALL(function, signature())
                        .WillOnce(ReturnRef(signature));

                EXPECT_CALL(*param_expression_ptr, type())
                        .WillRepeatedly(ReturnRef(param_type));

                EXPECT_CALL(errs.mock, notify_error(_));

                overload_resolution resolution{ errs.observer, call_token };
                const auto chosen = resolution.choose(lookup_result, param_expressions);

                EXPECT_THAT(chosen, IsNull());
            }

            TEST_F(OverloarResolutionTest, TriesInitializeReferenceWithTemporaryValue_RaiseErrorAndReturnNull)
            {
                errs_t errs;
                const auto call_token = token_identifier("foo");
                StrictMock<sema_function_mock> function;
                auto param_expression = expression_mock();
                auto param_expression_ptr = param_expression.get();
                std::vector<std::unique_ptr<expression_node>> param_expressions;
                param_expressions.emplace_back(std::move(param_expression));

                single_scope_function_lookup_result_t scope_result{ &function };
                function_lookup_result_t lookup_result{ scope_result };

                const auto function_name_token = token_identifier("foo");
                const auto param_name_token = token_identifier("param");
                const sema_type function_param_type{ sema_type_reference{ valid_type } };
                function_signature signature{
                        function_name_token,
                        { parameter_declaration{ function_param_type, param_name_token } }
                };
                EXPECT_CALL(function, signature())
                        .WillOnce(ReturnRef(signature));

                EXPECT_CALL(*param_expression_ptr, type())
                        .WillRepeatedly(ReturnRef(valid_type));

                EXPECT_CALL(*param_expression_ptr, produces_temporary_value())
                        .WillOnce(Return(true));

                EXPECT_CALL(errs.mock, notify_error(_));

                overload_resolution resolution{ errs.observer, call_token };
                const auto chosen = resolution.choose(lookup_result, param_expressions);

                EXPECT_THAT(chosen, IsNull());
            }

            TEST_F(OverloarResolutionTest, ParamsMatch_ReturnFunction)
            {
                errs_t errs;
                const auto call_token = token_identifier("foo");
                StrictMock<sema_function_mock> function;
                auto param_expression = expression_mock();
                auto param_expression_ptr = param_expression.get();
                std::vector<std::unique_ptr<expression_node>> param_expressions;
                param_expressions.emplace_back(std::move(param_expression));

                single_scope_function_lookup_result_t scope_result{ &function };
                function_lookup_result_t lookup_result{ scope_result };

                const auto function_name_token = token_identifier("foo");
                const auto param_name_token = token_identifier("param");
                function_signature signature{
                        function_name_token,
                        { parameter_declaration{ valid_type, param_name_token } }
                };
                EXPECT_CALL(function, signature())
                        .WillOnce(ReturnRef(signature));

                EXPECT_CALL(*param_expression_ptr, type())
                        .WillRepeatedly(ReturnRef(valid_type));

                overload_resolution resolution{ errs.observer, call_token };
                const auto chosen = resolution.choose(lookup_result, param_expressions);

                EXPECT_THAT(chosen, NotNull());
                EXPECT_THAT(chosen, Eq(&function));
            }

            TEST_F(OverloarResolutionTest, GoodFunctionInUpperScopeHidByBadOne_RaiseErrorAndReturnNull)
            {
                errs_t errs;
                const auto call_token = token_identifier("foo");
                StrictMock<sema_function_mock> good_function;
                StrictMock<sema_function_mock> bad_function;
                auto param_expression = expression_mock();
                auto param_expression_ptr = param_expression.get();
                std::vector<std::unique_ptr<expression_node>> param_expressions;
                param_expressions.emplace_back(std::move(param_expression));

                single_scope_function_lookup_result_t scope_with_good_function{ &good_function };
                single_scope_function_lookup_result_t scope_with_bad_function{ &bad_function };
                function_lookup_result_t lookup_result{ scope_with_bad_function,
                                                        scope_with_good_function };

                const auto function_name_token = token_identifier("foo");
                const auto param_name_token = token_identifier("param");
                const sema_type bad_function_param_type{ valid_context, ast::type_representation{ param_name_token }, {} };
                function_signature bad_function_signature{
                        function_name_token,
                        { parameter_declaration{ bad_function_param_type, param_name_token } }
                };
                EXPECT_CALL(bad_function, signature())
                        .WillOnce(ReturnRef(bad_function_signature));

                EXPECT_CALL(*param_expression_ptr, type())
                        .WillRepeatedly(ReturnRef(valid_type));

                EXPECT_CALL(errs.mock, notify_error(_));

                overload_resolution resolution{ errs.observer, call_token };
                const auto chosen = resolution.choose(lookup_result, param_expressions);

                EXPECT_THAT(chosen, IsNull());
            }

            TEST_F(OverloarResolutionTest, GoodFunctionInUpperScope_ReturnFunction)
            {
                errs_t errs;
                const auto call_token = token_identifier("foo");
                StrictMock<sema_function_mock> function;
                auto param_expression = expression_mock();
                auto param_expression_ptr = param_expression.get();
                std::vector<std::unique_ptr<expression_node>> param_expressions;
                param_expressions.emplace_back(std::move(param_expression));

                single_scope_function_lookup_result_t scope_result{ &function };
                function_lookup_result_t lookup_result{ single_scope_function_lookup_result_t{},
                                                        scope_result };

                const auto function_name_token = token_identifier("foo");
                const auto param_name_token = token_identifier("param");
                function_signature signature{
                        function_name_token,
                        { parameter_declaration{ valid_type, param_name_token } }
                };
                EXPECT_CALL(function, signature())
                        .WillOnce(ReturnRef(signature));

                EXPECT_CALL(*param_expression_ptr, type())
                        .WillRepeatedly(ReturnRef(valid_type));

                overload_resolution resolution{ errs.observer, call_token };
                const auto chosen = resolution.choose(lookup_result, param_expressions);

                EXPECT_THAT(chosen, NotNull());
                EXPECT_THAT(chosen, Eq(&function));
            }

            TEST_F(OverloarResolutionTest, GoodOveloadedFunction_ReturnFunction)
            {
                errs_t errs;
                const auto call_token = token_identifier("foo");
                StrictMock<sema_function_mock> good_function;
                StrictMock<sema_function_mock> bad_function;
                auto param_expression = expression_mock();
                auto param_expression_ptr = param_expression.get();
                std::vector<std::unique_ptr<expression_node>> param_expressions;
                param_expressions.emplace_back(std::move(param_expression));

                single_scope_function_lookup_result_t scope{ &bad_function, &good_function };
                function_lookup_result_t lookup_result{ scope };

                const auto function_name_token = token_identifier("foo");
                const auto param_name_token = token_identifier("param");

                const sema_type bad_function_param_type{ valid_context, ast::type_representation{ param_name_token }, {} };
                function_signature bad_function_signature{
                        function_name_token,
                        { parameter_declaration{ bad_function_param_type, param_name_token } }
                };
                EXPECT_CALL(bad_function, signature())
                        .WillOnce(ReturnRef(bad_function_signature));

                function_signature good_function_signature{
                        function_name_token,
                        { parameter_declaration{ valid_type, param_name_token } }
                };
                EXPECT_CALL(good_function, signature())
                        .WillOnce(ReturnRef(good_function_signature));

                EXPECT_CALL(*param_expression_ptr, type())
                        .WillRepeatedly(ReturnRef(valid_type));

                overload_resolution resolution{ errs.observer, call_token };
                const auto chosen = resolution.choose(lookup_result, param_expressions);

                EXPECT_THAT(chosen, NotNull());
                EXPECT_THAT(chosen, Eq(&good_function));
            }
}
