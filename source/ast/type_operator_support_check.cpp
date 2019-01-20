#include "ast/type_operator_support_check.hpp"
#include "ast/type_kind.hpp"
#include "lexer/token/token_type.hpp"
#include "type_operator_support_check.hpp"


namespace cmsl
{
    namespace ast
    {
        type_operator_support_check::type_operator_support_check()
            : m_types_operators_support{ get_types_operators_support() }
        {}

        type_operator_support_check::types_operators_support_t type_operator_support_check::get_types_operators_support() const
        {
            const auto bitwise_operators = {
                    token_type_t::amp,
                    token_type_t::ampequal,
                    token_type_t::pipe,
                    token_type_t::pipeequal,
                    token_type_t::xor_,
                    token_type_t::xorequal
            };

            const auto arithmetic_multiplicative_operators = {
                    token_type_t::star,
                    token_type_t::starequal,
                    token_type_t::slash,
                    token_type_t::slashequal
            };

            const auto integral_multiplicative_operators = {
                    token_type_t::percent,
                    token_type_t::percentequal
            };

            const auto additive_operators = {
                    token_type_t::plus,
                    token_type_t::plusequal,
                    token_type_t::minus,
                    token_type_t::minusequal
            };

            const auto logical_operators = {
                    token_type_t::less,
                    token_type_t::lessequal,
                    token_type_t::greater,
                    token_type_t::greaterequal,
                    token_type_t::ampamp,
                    token_type_t::pipepipe,
                    token_type_t::equalequal,
                    token_type_t::exclaimequal
            };

            return types_operators_support_t{
                    {
                        type_kind::k_bool,
                        make_group(bitwise_operators,
                                   arithmetic_multiplicative_operators,
                                   integral_multiplicative_operators,
                                   additive_operators,
                                   logical_operators)
                    },
                    {
                        type_kind::k_int,
                        make_group(bitwise_operators,
                                   arithmetic_multiplicative_operators,
                                   integral_multiplicative_operators,
                                   additive_operators,
                                   logical_operators)
                    },
                    {
                        type_kind::k_double,
                        make_group(arithmetic_multiplicative_operators,
                                   additive_operators,
                                   logical_operators)
                    },
                    {
                        type_kind::k_string,
                        get_string_operators_support()
                    }
            };
        }

        type_operator_support_check::operators_group_t type_operator_support_check::get_string_operators_support() const
        {
            return operators_group_t {
                    token_type_t::plus,
                    token_type_t::plusequal,

                    token_type_t::less,
                    token_type_t::lessequal,
                    token_type_t::greater,
                    token_type_t::greaterequal,
                    token_type_t::equalequal,
                    token_type_t::exclaimequal
            };
        }

        bool type_operator_support_check::type_supports_operator(type_kind t,
                                                                 type_operator_support_check::token_type_t op) const
                                                                 {
            auto group_pair = m_types_operators_support.find(t);
            if(group_pair == std::cend(m_types_operators_support))
            {
                return false;
            }

            const auto& group = group_pair->second;
            return group.find(op) != std::cend(group);
        }
    }
}