//#pragma once
//
//#include "ast/type.hpp"
//
//namespace cmsl
//{
//    namespace ast
//    {
//        enum class type_kind;
//
//        class integer_constant : public type
//        {
//        public:
//            using token_type_t = lexer::token::token_type;
//
//            explicit integer_constant(lexer::token::token token);
//            explicit integer_constant(cmsl::string_view name, type_kind kind);
//
//            bool is_builtin() const;
//
//            cmsl::string_view get_name() const;
//
//        private:
//            cmsl::string_view m_name;
//            type_kind m_kind;
//        };
//    }
//}
