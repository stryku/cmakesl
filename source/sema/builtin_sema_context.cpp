#include "sema/builtin_sema_context.hpp"
#include "sema/factories.hpp"
#include "sema/type_builder.hpp"
#include "sema/builtin_function_kind.hpp"
#include "builtin_sema_context.hpp"


namespace cmsl
{

    namespace sema
    {
        builtin_sema_context::builtin_sema_context(sema_type_factory &type_factory,
                                                   sema_function_factory &function_factory,
                                                   sema_context_factory &context_factory)
            : m_type_factory{ type_factory }
            , m_function_factory{ function_factory }
            , m_context_factory{ context_factory }
        {
            add_types();
            add_functions();
        }

        template<unsigned N>
        lexer::token::token builtin_sema_context::make_token(lexer::token::token_type token_type, const char (&tok)[N])
        {
            // N counts also '\0'
            const auto src_range = source_range{
                    source_location{ 1u, 1u, 0u },
                    source_location{ 1u, N, N - 1u }
            };
            return lexer::token::token{ token_type, src_range, tok };
        }

        template<unsigned N>
        lexer::token::token builtin_sema_context::make_id_token(const char (&tok)[N])
        {
            return make_token(token_type_t::identifier, tok);
        }

        void builtin_sema_context::add_types()
        {
            auto bool_manipulator = add_bool_type();
            auto int_manipulator = add_int_type();
            auto double_manipulator = add_double_type();
            auto string_manipulator = add_string_type();

            add_bool_member_functions(bool_manipulator);
            add_int_member_functions(int_manipulator);
            add_double_member_functions(double_manipulator);
            add_string_member_functions(string_manipulator);
        }

        void builtin_sema_context::add_functions()
        {

        }

        template<typename Functions>
        void builtin_sema_context::add_type_member_functions(type_builder &manipulator, Functions &&functions)
        {
            for(const auto& function : functions)
            {
                manipulator.with_builtin_function(function.return_type,
                                                  std::move(function.signature),
                                                  function.kind);
            }
        }

        type_builder builtin_sema_context::add_bool_type()
        {
            const auto token = make_token(token_type_t::kw_bool, "bool");
            type_builder builder{ m_type_factory, m_function_factory, m_context_factory, *this, token };
            builder.build_and_register_in_context();
            return builder;
        }

        void builtin_sema_context::add_bool_member_functions(type_builder &bool_manipulator)
        {
            const auto& int_type = *find_type("int");
            const auto& bool_type = *find_type("bool");

            const auto functions = {
                builtin_function_info{ // bool()
                        bool_type,
                        function_signature{ make_id_token("bool"), {} },
                        builtin_function_kind::bool_ctor
                },
                builtin_function_info{ // bool(bool)
                        bool_type,
                        function_signature{ make_id_token("bool"),
                                            { parameter_declaration{bool_type, make_id_token("") } } },
                        builtin_function_kind::bool_ctor_bool
                },
                builtin_function_info{ // bool(int)
                        bool_type,
                        function_signature{ make_id_token("bool"),
                                            { parameter_declaration{int_type, make_id_token("") } } },
                        builtin_function_kind::bool_ctor_int
                },
                builtin_function_info{ // operator=(bool)
                        bool_type,
                        function_signature{ make_token(token_type_t::equal, "="),
                                            { parameter_declaration{bool_type, make_id_token("") } } },
                        builtin_function_kind::bool_operator_equal
                },
                builtin_function_info{ // operator||(bool)
                        bool_type,
                        function_signature{ make_token(token_type_t::pipepipe, "||"),
                                            { parameter_declaration{bool_type, make_id_token("") } } },
                        builtin_function_kind::bool_operator_pipe_pipe
                },
                builtin_function_info{ // operator&&(bool)
                        bool_type,
                        function_signature{ make_token(token_type_t::ampamp, "&&"),
                                            { parameter_declaration{bool_type, make_id_token("") } } },
                        builtin_function_kind::bool_operator_amp_amp
                },
                builtin_function_info{ // to_string()
                        *find_type("string"),
                        function_signature{ make_id_token("to_string"), {} },
                        builtin_function_kind::bool_to_string
                }
            };

            add_type_member_functions(bool_manipulator, functions);
        }

        type_builder builtin_sema_context::add_int_type()
        {
            const auto token = make_token(token_type_t::kw_int, "int");
            type_builder builder{ m_type_factory, m_function_factory, m_context_factory, *this, token };
            builder.build_and_register_in_context();
            return builder;
        }

        void builtin_sema_context::add_int_member_functions(type_builder &int_manipulator)
        {
            const auto& int_type = *find_type("int");
            const auto& bool_type = *find_type("bool");

            const auto functions = {
                    builtin_function_info{ // int()
                            int_type,
                            function_signature{ make_id_token("int"), {} },
                            builtin_function_kind::int_ctor
                    },
                    builtin_function_info{ // int(bool)
                            int_type,
                            function_signature{ make_id_token("int"),
                                                { parameter_declaration{bool_type, make_id_token("") } } },
                            builtin_function_kind::int_ctor_bool
                    },
                    builtin_function_info{ // int(int)
                            int_type,
                            function_signature{ make_id_token("int"),
                                                { parameter_declaration{int_type, make_id_token("") } } },
                            builtin_function_kind::int_ctor_int
                    },
                    builtin_function_info{ // int(double)
                            int_type,
                            function_signature{ make_id_token("int"),
                                                { parameter_declaration{*find_type("double"), make_id_token("") } } },
                            builtin_function_kind::int_ctor_double
                    },
                    builtin_function_info{ // to_string()
                            *find_type("string"),
                            function_signature{ make_id_token("to_string"), {} },
                            builtin_function_kind::int_to_string
                    },
                    builtin_function_info{ // operator+(int)
                            int_type,
                            function_signature{ make_token(token_type_t::plus, "+"),
                                                { parameter_declaration{int_type, make_id_token("") } } },
                            builtin_function_kind::int_operator_plus
                    },
                    builtin_function_info{ // operator-(int)
                            int_type,
                            function_signature{ make_token(token_type_t::minus, "-"),
                                                { parameter_declaration{int_type, make_id_token("") } } },
                            builtin_function_kind::int_operator_minus
                    },
                    builtin_function_info{ // operator*(int)
                            int_type,
                            function_signature{ make_token(token_type_t::star, "*"),
                                                { parameter_declaration{int_type, make_id_token("") } } },
                            builtin_function_kind::int_operator_star
                    },
                    builtin_function_info{ // operator/(int)
                            int_type,
                            function_signature{ make_token(token_type_t::slash, "/"),
                                                { parameter_declaration{int_type, make_id_token("") } } },
                            builtin_function_kind::int_operator_slash
                    },
                    builtin_function_info{ // operator=(int)
                            int_type,
                            function_signature{ make_token(token_type_t::equal, "="),
                                                { parameter_declaration{int_type, make_id_token("") } } },
                            builtin_function_kind::int_operator_equal
                    },
                    builtin_function_info{ // operator+=(int)
                            int_type,
                            function_signature{ make_token(token_type_t::plusequal, "+="),
                                                { parameter_declaration{int_type, make_id_token("") } } },
                            builtin_function_kind::int_operator_plus_equal
                    },
                    builtin_function_info{ // operator-=(int)
                            int_type,
                            function_signature{ make_token(token_type_t::minusequal, "-="),
                                                { parameter_declaration{int_type, make_id_token("") } } },
                            builtin_function_kind::int_operator_minus_equal
                    },
                    builtin_function_info{ // operator*=(int)
                            int_type,
                            function_signature{ make_token(token_type_t::starequal, "*="),
                                                { parameter_declaration{int_type, make_id_token("") } } },
                            builtin_function_kind::int_operator_star_equal
                    },
                    builtin_function_info{ // operator/=(int)
                            int_type,
                            function_signature{ make_token(token_type_t::slashequal, "/="),
                                                { parameter_declaration{int_type, make_id_token("") } } },
                            builtin_function_kind::int_operator_slash_equal
                    },
                    builtin_function_info{ // operator<(int)
                            bool_type,
                            function_signature{ make_token(token_type_t::less, "<"),
                                                { parameter_declaration{int_type, make_id_token("") } } },
                            builtin_function_kind::int_operator_less
                    },
                    builtin_function_info{ // operator<=(int)
                            bool_type,
                            function_signature{ make_token(token_type_t::lessequal, "<="),
                                                { parameter_declaration{int_type, make_id_token("") } } },
                            builtin_function_kind::int_operator_less_equal
                    },
                    builtin_function_info{ // operator>(int)
                            bool_type,
                            function_signature{ make_token(token_type_t::greater, ">"),
                                                { parameter_declaration{int_type, make_id_token("") } } },
                            builtin_function_kind::int_operator_greater
                    },
                    builtin_function_info{ // operator>=(int)
                            bool_type,
                            function_signature{ make_token(token_type_t::greaterequal, ">="),
                                                { parameter_declaration{int_type, make_id_token("") } } },
                            builtin_function_kind::int_operator_greater_equal
                    },
                    builtin_function_info{ // operator==(int)
                            bool_type,
                            function_signature{ make_token(token_type_t::equalequal, "=="),
                                                { parameter_declaration{int_type, make_id_token("") } } },
                            builtin_function_kind::int_operator_equal_equal
                    }
            };

            add_type_member_functions(int_manipulator, functions);
        }

        type_builder builtin_sema_context::add_double_type()
        {
            const auto token = make_token(token_type_t::kw_double, "double");
            type_builder builder{ m_type_factory, m_function_factory, m_context_factory, *this, token };
            builder.build_and_register_in_context();
            return builder;
        }

        void builtin_sema_context::add_double_member_functions(type_builder &double_manipulator)
        {
            const auto& double_type = *find_type("double");
            const auto& bool_type = *find_type("bool");

            const auto functions = {
                    builtin_function_info{ // double()
                            double_type,
                            function_signature{ make_id_token("double"), {} },
                            builtin_function_kind::double_ctor
                    },
                    builtin_function_info{ // double(double)
                            double_type,
                            function_signature{ make_id_token("double"),
                                                { parameter_declaration{double_type, make_id_token("") } } },
                            builtin_function_kind::double_ctor_double
                    },
                    builtin_function_info{ // double(int)
                            double_type,
                            function_signature{ make_id_token("double"),
                                                { parameter_declaration{*find_type("int"), make_id_token("") } } },
                            builtin_function_kind::double_ctor_int
                    },
                    builtin_function_info{ // operator+(double)
                            double_type,
                            function_signature{ make_token(token_type_t::plus, "+"),
                                                { parameter_declaration{double_type, make_id_token("") } } },
                            builtin_function_kind::double_operator_plus
                    },
                    builtin_function_info{ // operator-(double)
                            double_type,
                            function_signature{ make_token(token_type_t::minus, "-"),
                                                { parameter_declaration{double_type, make_id_token("") } } },
                            builtin_function_kind::double_operator_minus
                    },
                    builtin_function_info{ // operator*(double)
                            double_type,
                            function_signature{ make_token(token_type_t::star, "*"),
                                                { parameter_declaration{double_type, make_id_token("") } } },
                            builtin_function_kind::double_operator_star
                    },
                    builtin_function_info{ // operator/(double)
                            double_type,
                            function_signature{ make_token(token_type_t::slash, "/"),
                                                { parameter_declaration{double_type, make_id_token("") } } },
                            builtin_function_kind::double_operator_slash
                    },
                    builtin_function_info{ // operator=(double)
                            double_type,
                            function_signature{ make_token(token_type_t::equal, "="),
                                                { parameter_declaration{double_type, make_id_token("") } } },
                            builtin_function_kind::double_operator_equal
                    },
                    builtin_function_info{ // operator+=(double)
                            double_type,
                            function_signature{ make_token(token_type_t::plusequal, "+="),
                                                { parameter_declaration{double_type, make_id_token("") } } },
                            builtin_function_kind::double_operator_plus_equal
                    },
                    builtin_function_info{ // operator-=(double)
                            double_type,
                            function_signature{ make_token(token_type_t::minusequal, "-="),
                                                { parameter_declaration{double_type, make_id_token("") } } },
                            builtin_function_kind::double_operator_minus_equal
                    },
                    builtin_function_info{ // operator*=(double)
                            double_type,
                            function_signature{ make_token(token_type_t::starequal, "*="),
                                                { parameter_declaration{double_type, make_id_token("") } } },
                            builtin_function_kind::double_operator_star_equal
                    },
                    builtin_function_info{ // operator/=(double)
                            double_type,
                            function_signature{ make_token(token_type_t::slashequal, "/="),
                                                { parameter_declaration{double_type, make_id_token("") } } },
                            builtin_function_kind::double_operator_slash_equal
                    },
                    builtin_function_info{ // operator<(double)
                            bool_type,
                            function_signature{ make_token(token_type_t::less, "<"),
                                                { parameter_declaration{double_type, make_id_token("") } } },
                            builtin_function_kind::double_operator_less
                    },
                    builtin_function_info{ // operator<=(double)
                            bool_type,
                            function_signature{ make_token(token_type_t::lessequal, "<="),
                                                { parameter_declaration{double_type, make_id_token("") } } },
                            builtin_function_kind::double_operator_less_equal
                    },
                    builtin_function_info{ // operator>(double)
                            bool_type,
                            function_signature{ make_token(token_type_t::greater, ">"),
                                                { parameter_declaration{double_type, make_id_token("") } } },
                            builtin_function_kind::double_operator_greater
                    },
                    builtin_function_info{ // operator>=(double)
                            bool_type,
                            function_signature{ make_token(token_type_t::greaterequal, ">="),
                                                { parameter_declaration{double_type, make_id_token("") } } },
                            builtin_function_kind::double_operator_greater_equal
                    },
                    builtin_function_info{
                            *find_type("string"),
                            function_signature{ make_id_token("to_string"), {} },
                            builtin_function_kind::double_to_string
                    }
            };

            add_type_member_functions(double_manipulator, functions);
        }

        type_builder builtin_sema_context::add_string_type()
        {
            const auto token = make_token(token_type_t::kw_string, "string");
            type_builder builder{ m_type_factory, m_function_factory, m_context_factory, *this, token };
            builder.build_and_register_in_context();
            return builder;
        }

        void builtin_sema_context::add_string_member_functions(type_builder &string_manipulator)
        {
            const auto functions = {
                    builtin_function_info{
                            *find_type("bool"),
                            function_signature{ make_id_token("empty"), {} },
                            builtin_function_kind::string_empty
                    },
                    builtin_function_info{
                            *find_type("int"),
                            function_signature{ make_id_token("size"), {} },
                            builtin_function_kind::string_size
                    }

            };

            add_type_member_functions(string_manipulator, functions);
        }
    }
}
