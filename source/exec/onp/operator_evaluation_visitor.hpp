#pragma once

#include "common/assert.hpp"
#include "exec/instance/instance_value.hpp"
#include "lexer/token/token_type.hpp"

#include <boost/variant.hpp>

namespace cmsl
{
    namespace exec
    {
        namespace onp
        {
            namespace details
            {
                struct operator_evaluation_visitor : public boost::static_visitor<inst::instance_value_t>
                {
                    explicit operator_evaluation_visitor(lexer::token::token_type op)
                        : m_operator{ op }
                    {}

                protected:
                    lexer::token::token_type m_operator;
                };

                template <typename T>
                using enable_integral = std::enable_if_t<std::is_integral<std::decay_t<T>>::value>;
            }

            // At the point of visitor call, lhs and rhs instances are already converted to the same type,
            // in other words std::is_same_v<Lhs, Rhs> == true

            struct additive_operator_evaluation_visitor : public details::operator_evaluation_visitor
            {
                additive_operator_evaluation_visitor(lexer::token::token_type op)
                    : details::operator_evaluation_visitor{ op }
                {}

                template <typename ValuesType>
                inst::instance_value_t operator()(ValuesType&& lhs, ValuesType&& rhs)
                {
                    switch(m_operator)
                    {
                        case lexer::token::token_type::minus: return lhs - rhs;
                        case lexer::token::token_type::plus: return lhs + rhs;

                        default:
                            CMSL_UNREACHABLE("Unsupported operator");
                    }
                }

                template <typename T, typename T2>
                inst::instance_value_t operator()(T&&, T2&&)
                {
                    // This one should never be called
                    return 1;
                };
            };

            struct multiplicative_operator_evaluation_visitor : public details::operator_evaluation_visitor
            {
                multiplicative_operator_evaluation_visitor(lexer::token::token_type op)
                        : details::operator_evaluation_visitor{ op }
                {}

                template <typename ValuesType>
                inst::instance_value_t operator()(ValuesType&& lhs, ValuesType&& rhs)
                {
                    switch(m_operator)
                    {
                        case lexer::token::token_type::slash: return lhs / rhs;
                        case lexer::token::token_type::star: return lhs * rhs;

                        default:
                            CMSL_UNREACHABLE("Unsupported operator");
                    }
                }

                template <typename T, typename T2>
                inst::instance_value_t operator()(T&&, T2&&)
                {
                    // This one should never be called
                    return 1;
                };
            };

            struct logical_operator_evaluation_visitor : public details::operator_evaluation_visitor
            {
                logical_operator_evaluation_visitor(lexer::token::token_type op)
                        : details::operator_evaluation_visitor{ op }
                {}

                template <typename ValuesType>
                inst::instance_value_t operator()(ValuesType&& lhs, ValuesType&& rhs)
                {
                    switch(m_operator)
                    {
                        case lexer::token::token_type::equalequal: return lhs == rhs;
                        case lexer::token::token_type::ampamp: return lhs && rhs;
                        case lexer::token::token_type::pipepipe: return lhs || rhs;
                        case lexer::token::token_type::less: return lhs < rhs;
                        case lexer::token::token_type::lessequal: return lhs <= rhs;
                        case lexer::token::token_type::greater: return lhs > rhs;
                        case lexer::token::token_type::greaterequal: return lhs >= rhs;

                        default:
                            CMSL_UNREACHABLE("Unsupported operator");
                    }
                }

                template <typename T, typename T2>
                inst::instance_value_t operator()(T&&, T2&&)
                {
                    // This one should never be called
                    return 1;
                };
            };

            struct reminder_operator_evaluation_visitor : public details::operator_evaluation_visitor
            {
                template <typename T>
                using enable_reminder = details::enable_integral<T>;

                reminder_operator_evaluation_visitor(lexer::token::token_type op)
                        : details::operator_evaluation_visitor{ op }
                {}

                template <typename ValuesType,
                          typename = enable_reminder<ValuesType>>
                inst::instance_value_t operator()(ValuesType&& lhs, ValuesType&& rhs)
                {
                    switch(m_operator)
                    {
                        case lexer::token::token_type::percent: return lhs % rhs;

                        default:
                            CMSL_UNREACHABLE("Unsupported operator");
                    }
                }

                template <typename T, typename T2>
                inst::instance_value_t operator()(T&&, T2&&)
                {
                    // This one should never be called
                    return 1;
                };
            };

            struct bitwise_operator_evaluation_visitor : public details::operator_evaluation_visitor
            {
                template <typename T>
                using enable_bitwise = details::enable_integral<T>;

                bitwise_operator_evaluation_visitor(lexer::token::token_type op)
                    : details::operator_evaluation_visitor{ op }
                {}

                template <typename ValuesType,
                        typename = enable_bitwise<ValuesType>>
                inst::instance_value_t operator()(ValuesType&& lhs, ValuesType&& rhs)
                {
                    switch(m_operator)
                    {
                        case lexer::token::token_type::amp: return lhs & rhs;
                        case lexer::token::token_type::pipe: return lhs | rhs;
                        case lexer::token::token_type::xor_: return lhs ^ rhs;

                        default:
                            CMSL_UNREACHABLE("Unsupported operator");
                    }
                }

                template <typename T, typename T2>
                inst::instance_value_t operator()(T&&, T2&&)
                {
                    // This one should never be called
                    return 1;
                };
            };
        }
    }
}
