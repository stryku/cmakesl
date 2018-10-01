#pragma once

#include "common/assert.hpp"
#include "exec/instance/instance_value.hpp"
#include "lexer/token/token_type.hpp"
#include "exec/instance/instance.hpp"

#include <boost/variant.hpp>

namespace cmsl
{
    namespace exec
    {
        namespace onp
        {
            namespace details
            {
                struct operator_evaluation_visitor
                {
                    template <typename StoredValuesType, typename ValuesType, typename Operation>
                    inst::instance_value_t apply(ValuesType&& lhs, ValuesType&& rhs, Operation&& op)
                    {
                        return op(boost::get<StoredValuesType>(lhs), boost::get<StoredValuesType>(rhs));
                    };

                    template <typename StoredValuesType, typename DesiredType, typename ValuesType, typename Operation>
                    inst::instance_value_t apply_with_cast(ValuesType&& lhs, ValuesType&& rhs, Operation&& op)
                    {
                        const auto casted_lhs = static_cast<DesiredType>(boost::get<StoredValuesType>(lhs));
                        const auto casted_rhs = static_cast<DesiredType>(boost::get<StoredValuesType>(lhs));
                        return op(casted_lhs, casted_rhs);
                    };

                    template <typename ValuesType, typename Operation>
                    inst::instance_value_t apply_generic(ValuesType&& lhs, ValuesType&& rhs, Operation&& op)
                    {
                        const auto& generic_lhs = boost::get<inst::generic_instance_value>(lhs);
                        const auto& generic_rhs = boost::get<inst::generic_instance_value>(rhs);

                        // Visitor for underlying values
                        auto visitor = [op](const auto& lhs, const auto& rhs)
                        {
                            return op(lhs, rhs);
                        };

                        // Apply visitor for generics underlying values
                        return boost::apply_visitor(visitor,
                                                    generic_lhs.get_underlying_value_cref(),
                                                    generic_rhs.get_underlying_value_cref());
                    };
                };
            }

            // At the point of visitor call, lhs and rhs instances are already converted to the same type,
            // in other words std::is_same_v<Lhs, Rhs> == true
            // Another thing that we know is that expression values type supports given operator

            // Plus and minus operators need to be separated to different visitors because containers support plus but does not support minus
            struct plus_operator_evaluation_visitor : public details::operator_evaluation_visitor
            {
                inst::instance_value_t visit(const inst::instance_value_t& lhs, const inst::instance_value_t& rhs)
                {
                    const auto operation = [](const auto& lhs, const auto& rhs)
                    {
                        return lhs + rhs;
                    };

                    const auto values_type = inst::get_inst_val_type(lhs);
                    switch(values_type)
                    {
                        case inst::instance_value_type::bool_:
                            return apply_with_cast<bool, inst::int_t>(lhs, rhs, operation);
                        case inst::instance_value_type::int_:
                            return apply<inst::int_t>(lhs, rhs, operation);
                        case inst::instance_value_type::double_:
                            return apply<double>(lhs, rhs, operation);
                        case inst::instance_value_type::string:
                            return apply<std::string>(lhs, rhs, operation);

                        default:
                            CMSL_UNREACHABLE("Applying plus operator to types that doesn't support it.");
                    }
                }
            };

            struct minus_operator_evaluation_visitor : public details::operator_evaluation_visitor
            {
                inst::instance_value_t visit(const inst::instance_value_t& lhs, const inst::instance_value_t& rhs)
                {
                    const auto operation = [](const auto& lhs, const auto& rhs)
                    {
                        return lhs - rhs;
                    };

                    const auto values_type = inst::get_inst_val_type(lhs);
                    switch(values_type)
                    {
                        case inst::instance_value_type::bool_:
                            return apply_with_cast<bool, inst::int_t>(lhs, rhs, operation);
                        case inst::instance_value_type::int_:
                            return apply<inst::int_t>(lhs, rhs, operation);
                        case inst::instance_value_type::double_:
                            return apply<double>(lhs, rhs, operation);

                        default:
                            CMSL_UNREACHABLE("Applying minus operator to types that doesn't support it.");
                    }
                }
            };

            struct multiplicative_operator_evaluation_visitor : public details::operator_evaluation_visitor
            {
                multiplicative_operator_evaluation_visitor(lexer::token::token_type op)
                    : m_operator{ op }
                {}

                inst::instance_value_t visit(const inst::instance_value_t& lhs, const inst::instance_value_t& rhs)
                {
                    const auto operation = [op = m_operator](const auto& lhs, const auto& rhs)
                    {
                        switch(op)
                        {
                            case lexer::token::token_type::slash: return lhs / rhs;
                            case lexer::token::token_type::star: return lhs * rhs;

                            default:
                                CMSL_UNREACHABLE("Unsupported operator");
                        }
                    };

                    const auto values_type = inst::get_inst_val_type(lhs);
                    switch(values_type)
                    {
                        case inst::instance_value_type::bool_:
                            return apply_with_cast<bool, inst::int_t>(lhs, rhs, operation);
                        case inst::instance_value_type::int_:
                            return apply<inst::int_t>(lhs, rhs, operation);
                        case inst::instance_value_type::double_:
                            return apply<double>(lhs, rhs, operation);

                        default:
                            CMSL_UNREACHABLE("Applying multiplicative operator to types that doesn't support it.");
                    }
                }

            private:
                lexer::token::token_type m_operator;
            };

            // Separate ==, != and <, <=, >, >=. E.g. List supports == bot does not the other
            struct comparison_operator_evaluation_visitor : public details::operator_evaluation_visitor
            {
                comparison_operator_evaluation_visitor(lexer::token::token_type op)
                    : m_operator{ op }
                {}

                inst::instance_value_t visit(const inst::instance_value_t& lhs, const inst::instance_value_t& rhs)
                {
                    const auto operation = [op = m_operator](const auto& lhs, const auto& rhs)
                    {
                        switch(op)
                        {
                            case lexer::token::token_type::equalequal: return lhs == rhs;
                            case lexer::token::token_type::exclaimequal: return lhs != rhs;

                            default:
                                CMSL_UNREACHABLE("Unsupported operator");
                        }
                    };

                    const auto values_type = inst::get_inst_val_type(lhs);
                    switch(values_type)
                    {
                        case inst::instance_value_type::bool_:
                            return apply<bool>(lhs, rhs, operation);
                        case inst::instance_value_type::int_:
                            return apply<inst::int_t>(lhs, rhs, operation);
                        case inst::instance_value_type::double_:
                            return apply<double>(lhs, rhs, operation);
                        case inst::instance_value_type::string:
                            return apply<std::string>(lhs, rhs, operation);
                        case inst::instance_value_type::generic:
                            return apply_generic(lhs, rhs, operation);

                        default:
                            CMSL_UNREACHABLE("Applying equalequal operator to types that doesn't support it.");
                    }
                }

            private:
                lexer::token::token_type m_operator;
            };

            struct relation_operator_evaluation_visitor : public details::operator_evaluation_visitor
            {
                relation_operator_evaluation_visitor(lexer::token::token_type op)
                    : m_operator{ op }
                {}

                inst::instance_value_t visit(const inst::instance_value_t& lhs, const inst::instance_value_t& rhs)
                {
                    const auto operation = [op = m_operator](const auto& lhs, const auto& rhs)
                    {
                        switch(op)
                        {
                            case lexer::token::token_type::less: return lhs < rhs;
                            case lexer::token::token_type::lessequal: return lhs <= rhs;
                            case lexer::token::token_type::greater: return lhs > rhs;
                            case lexer::token::token_type::greaterequal: return lhs >= rhs;

                            default:
                                CMSL_UNREACHABLE("Unsupported operator");
                        }
                    };

                    const auto values_type = inst::get_inst_val_type(lhs);
                    switch(values_type)
                    {
                        case inst::instance_value_type::bool_:
                            return apply_with_cast<bool, inst::int_t>(lhs, rhs, operation);
                        case inst::instance_value_type::int_:
                            return apply<inst::int_t>(lhs, rhs, operation);
                        case inst::instance_value_type::double_:
                            return apply<double>(lhs, rhs, operation);
                        case inst::instance_value_type::string:
                            return apply<std::string>(lhs, rhs, operation);

                        default:
                            CMSL_UNREACHABLE("Applying relation operator to types that doesn't support it.");
                    }
                }

            private:
                lexer::token::token_type m_operator;
            };

            struct logical_operator_evaluation_visitor : public details::operator_evaluation_visitor
            {
                logical_operator_evaluation_visitor(lexer::token::token_type op)
                    : m_operator{ op }
                {}

                inst::instance_value_t visit(const inst::instance_value_t& lhs, const inst::instance_value_t& rhs)
                {
                    const auto operation = [op = m_operator](const auto& lhs, const auto& rhs)
                    {
                        switch(op)
                        {
                            case lexer::token::token_type::ampamp: return lhs && rhs;
                            case lexer::token::token_type::pipepipe: return lhs || rhs;

                            default:
                                CMSL_UNREACHABLE("Unsupported operator");
                        }
                    };

                    const auto values_type = inst::get_inst_val_type(lhs);
                    switch(values_type)
                    {
                        case inst::instance_value_type::bool_:
                            return apply<bool>(lhs, rhs, operation);
                        case inst::instance_value_type::int_:
                            return apply<inst::int_t>(lhs, rhs, operation);

                        default:
                            CMSL_UNREACHABLE("Applying logical operator to types that doesn't support it.");
                    }
                }

            private:
                lexer::token::token_type m_operator;
            };

            // Needs to be seperated because double doesn't support %
            struct reminder_operator_evaluation_visitor : public details::operator_evaluation_visitor
            {
                inst::instance_value_t visit(const inst::instance_value_t& lhs, const inst::instance_value_t& rhs)
                {
                    const auto operation = [](const auto& lhs, const auto& rhs)
                    {
                        return lhs % rhs;
                    };

                    const auto values_type = inst::get_inst_val_type(lhs);
                    switch(values_type)
                    {
                        case inst::instance_value_type::bool_:
                            return apply_with_cast<bool, inst::int_t>(lhs, rhs, operation);
                        case inst::instance_value_type::int_:
                            return apply<inst::int_t>(lhs, rhs, operation);

                        default:
                            CMSL_UNREACHABLE("Applying reminder operator to types that doesn't support it.");
                    }
                }
            };

            struct bitwise_operator_evaluation_visitor : public details::operator_evaluation_visitor
            {
                bitwise_operator_evaluation_visitor(lexer::token::token_type op)
                        : m_operator{ op }
                {}

                inst::instance_value_t visit(const inst::instance_value_t& lhs, const inst::instance_value_t& rhs)
                {
                    const auto operation = [op = m_operator](const auto& lhs, const auto& rhs)
                    {
                        switch(op)
                        {
                            case lexer::token::token_type::amp: return lhs & rhs;
                            case lexer::token::token_type::pipe: return lhs | rhs;
                            case lexer::token::token_type::xor_: return lhs ^ rhs;

                            default:
                                CMSL_UNREACHABLE("Unsupported operator");
                        }
                    };

                    const auto values_type = inst::get_inst_val_type(lhs);
                    switch(values_type)
                    {
                        case inst::instance_value_type::bool_:
                            return apply_with_cast<bool, inst::int_t>(lhs, rhs, operation);
                        case inst::instance_value_type::int_:
                            return apply<inst::int_t>(lhs, rhs, operation);

                        default:
                            CMSL_UNREACHABLE("Applying bitwise operator to types that doesn't support it.");
                    }
                }

            private:
                lexer::token::token_type m_operator;
            };
        }
    }
}
