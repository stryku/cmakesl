#pragma once

#include <lexer/token/token_type.hpp>
#include "common/enum_class_utils.hpp"
#include "type_kind.hpp"

namespace cmsl
{
    namespace lexer
    {
        namespace token
        {
            enum class token_type;
        }
    }

    namespace ast
    {
        enum class type_kind;

        class type_operator_support_check
        {
        private:
            using token_type_t = lexer::token::token_type;
            using operators_group_t = enum_unordered_set<token_type_t>;
            using types_operators_support_t = enum_unordered_map<type_kind, operators_group_t>;

        public:
            explicit type_operator_support_check();

            bool type_supports_operator(type_kind t, token_type_t op) const;

        private:
            types_operators_support_t get_types_operators_support() const;
            operators_group_t get_string_operators_support() const;

            template <typename... Groups>
            auto make_group(Groups&&... groups_to_insert) const
            {
                operators_group_t result_group;

                auto inserter = [&result_group](const auto& group_to_insert)
                {
                    std::copy(std::cbegin(group_to_insert), std::cend(group_to_insert),
                              std::inserter(result_group, std::end(result_group)));
                };

                // todo change to fold expression
                for(const auto& group : { groups_to_insert... })
                {
                    inserter(group);
                }

                return result_group;
            }

        private:
            types_operators_support_t m_types_operators_support;
        };
    }
}
