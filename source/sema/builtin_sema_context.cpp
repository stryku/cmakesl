#include "common/assert.hpp"
#include "sema/builtin_sema_context.hpp"
#include "sema/factories.hpp"
#include "sema/type_builder.hpp"
#include "sema/builtin_function_kind.hpp"
#include "sema/builtin_sema_function.hpp"

#include "sema/builtin_types_finder.hpp"
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
            auto version_manipulator = add_version_type();
            auto project_manipulator = add_project_type();

            add_bool_member_functions(bool_manipulator);
            add_int_member_functions(int_manipulator);
            add_double_member_functions(double_manipulator);
            add_string_member_functions(string_manipulator);
            add_version_member_functions(version_manipulator);
            add_project_member_functions(project_manipulator);
        }

        void builtin_sema_context::add_functions()
        {
            const auto types_finder = builtin_types_finder{ *this };
            const auto& version_type = types_finder.find_version();
            const auto& void_type = types_finder.find_void();

            const auto functions = {
                builtin_function_info{ // void cmake_minimum_required(version)
                        void_type,
                        function_signature{make_id_token("cmake_minimum_required"),
                                           { parameter_declaration{version_type, make_id_token("")}}},
                        builtin_function_kind::cmake_minimum_required
                }
            };

            for(const auto& function : functions)
            {
                const auto& created = m_function_factory.create_builtin(*this,
                                                                        function.return_type,
                                                                        std::move(function.signature),
                                                                        function.kind);
                add_function(created);
            }
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
            static const auto token = make_token(token_type_t::kw_bool, "bool");
            static const auto name_representation = ast::type_representation{ token };
            type_builder builder{ m_type_factory, m_function_factory, m_context_factory, *this, name_representation };
            builder.build_and_register_in_context();
            return builder;
        }

        void builtin_sema_context::add_bool_member_functions(type_builder &bool_manipulator)
        {
            const auto types_finder = builtin_types_finder{ *this };
            const auto& int_type = types_finder.find_int();
            const auto& bool_type = types_finder.find_bool();
            const auto& string_type = types_finder.find_string();

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
                builtin_function_info{ // operator==(bool)
                        bool_type,
                        function_signature{ make_token(token_type_t::equalequal, "=="),
                                            { parameter_declaration{bool_type, make_id_token("") } } },
                        builtin_function_kind::bool_operator_equal_equal
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
                        string_type,
                        function_signature{ make_id_token("to_string"), {} },
                        builtin_function_kind::bool_to_string
                }
            };

            add_type_member_functions(bool_manipulator, functions);
        }

        type_builder builtin_sema_context::add_int_type()
        {
            static const auto token = make_token(token_type_t::kw_int, "int");
            static const auto name_representation = ast::type_representation{ token };
            type_builder builder{ m_type_factory, m_function_factory, m_context_factory, *this, name_representation };
            builder.build_and_register_in_context();
            return builder;
        }

        void builtin_sema_context::add_int_member_functions(type_builder &int_manipulator)
        {
            const auto types_finder = builtin_types_finder{ *this };
            const auto& int_type = types_finder.find_int();
            const auto& bool_type = types_finder.find_bool();
            const auto& double_type = types_finder.find_double();
            const auto& string_type = types_finder.find_string();

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
                                                { parameter_declaration{double_type, make_id_token("") } } },
                            builtin_function_kind::int_ctor_double
                    },
                    builtin_function_info{ // to_string()
                            string_type,
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
            static const auto token = make_token(token_type_t::kw_double, "double");
            static const auto name_representation = ast::type_representation{ token };
            type_builder builder{ m_type_factory, m_function_factory, m_context_factory, *this, name_representation };
            builder.build_and_register_in_context();
            return builder;
        }

        void builtin_sema_context::add_double_member_functions(type_builder &double_manipulator)
        {
            const auto types_finder = builtin_types_finder{ *this };
            const auto& int_type = types_finder.find_int();
            const auto& bool_type = types_finder.find_bool();
            const auto& double_type = types_finder.find_double();
            const auto& string_type = types_finder.find_string();

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
                                                { parameter_declaration{int_type, make_id_token("") } } },
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
                            string_type,
                            function_signature{ make_id_token("to_string"), {} },
                            builtin_function_kind::double_to_string
                    }
            };

            add_type_member_functions(double_manipulator, functions);
        }

        type_builder builtin_sema_context::add_string_type()
        {
            static const auto token = make_token(token_type_t::kw_string, "string");
            static const auto name_representation = ast::type_representation{ token };
            type_builder builder{ m_type_factory, m_function_factory, m_context_factory, *this, name_representation };
            builder.build_and_register_in_context();
            return builder;
        }

        void builtin_sema_context::add_string_member_functions(type_builder &string_manipulator)
        {
            const auto types_finder = builtin_types_finder{ *this };
            const auto& int_type = types_finder.find_int();
            const auto& bool_type = types_finder.find_bool();
            const auto& double_type = types_finder.find_double();
            const auto& string_type = types_finder.find_string();
            const auto& void_type = types_finder.find_string();


            const auto functions = {
                    builtin_function_info{ // string()
                            string_type,
                            function_signature{ make_id_token("string"), {} },
                            builtin_function_kind::string_ctor
                    },
                    builtin_function_info{ // string(string)
                            string_type,
                            function_signature{ make_id_token("string"),
                                                { parameter_declaration{string_type, make_id_token("") } } },
                            builtin_function_kind::string_ctor_string
                    },
                    builtin_function_info{ // string(string str, int count)
                            string_type,
                            function_signature{ make_id_token("string"),
                                                { parameter_declaration{string_type, make_id_token("") },
                                                  parameter_declaration{int_type, make_id_token("") }} },
                            builtin_function_kind::string_ctor_string_count
                    },
                    builtin_function_info{ // bool empty()
                            bool_type,
                            function_signature{ make_id_token("empty"), {} },
                            builtin_function_kind::string_empty
                    },
                    builtin_function_info{ // int size()
                            int_type,
                            function_signature{ make_id_token("size"), {} },
                            builtin_function_kind::string_size
                    },
                    builtin_function_info{ // bool operator==(string)
                            bool_type,
                            function_signature{ make_id_token("=="),
                                                { parameter_declaration{string_type, make_id_token("") } } },
                            builtin_function_kind::string_operator_equal_equal
                    },
                    builtin_function_info{ // bool operator!=(string)
                            bool_type,
                            function_signature{ make_id_token("!="),
                                                { parameter_declaration{string_type, make_id_token("") } } },
                            builtin_function_kind::string_operator_not_equal
                    },
                    builtin_function_info{ // bool operator<(string)
                            bool_type,
                            function_signature{ make_id_token("<"),
                                                { parameter_declaration{string_type, make_id_token("") } } },
                            builtin_function_kind::string_operator_less
                    },
                    builtin_function_info{ // bool operator<=(string)
                            bool_type,
                            function_signature{ make_id_token("<="),
                                                { parameter_declaration{string_type, make_id_token("") } } },
                            builtin_function_kind::string_operator_less_equal
                    },
                    builtin_function_info{ // bool operator>(string)
                            bool_type,
                            function_signature{ make_id_token(">"),
                                                { parameter_declaration{string_type, make_id_token("") } } },
                            builtin_function_kind::string_operator_greater
                    },
                    builtin_function_info{ // bool operator>=(string)
                            bool_type,
                            function_signature{ make_id_token(">="),
                                                { parameter_declaration{string_type, make_id_token("") } } },
                            builtin_function_kind::string_operator_greater_equal
                    },
                    builtin_function_info{ // string operator+(string)
                            string_type,
                            function_signature{ make_id_token("+"),
                                                { parameter_declaration{string_type, make_id_token("") } } },
                            builtin_function_kind::string_operator_plus
                    },
                    builtin_function_info{ // string& operator+=(string)
                            string_type, // Todo: reference
                            function_signature{ make_id_token("+="),
                                                { parameter_declaration{string_type, make_id_token("") } } },
                            builtin_function_kind::string_operator_plus_equal
                    },
                    builtin_function_info{ // void clear()
                            void_type,
                            function_signature{ make_id_token("clear"), {} },
                            builtin_function_kind::string_clear
                    },
                    builtin_function_info{ // string& insert(int position, string str)
                            string_type, // Todo: reference
                            function_signature{ make_id_token("insert"),
                                                { parameter_declaration{int_type, make_id_token("") },
                                                  parameter_declaration{string_type, make_id_token("") }} },
                            builtin_function_kind::string_insert_pos_str
                    },
                    builtin_function_info{ // string& erase(int position)
                            string_type, // Todo: reference
                            function_signature{ make_id_token("erase"),
                                                { parameter_declaration{int_type, make_id_token("") } } },
                            builtin_function_kind::string_erase_pos
                    },
                    builtin_function_info{ // string& erase(int position, int count)
                            string_type, // Todo: reference
                            function_signature{ make_id_token("erase"),
                                                { parameter_declaration{int_type, make_id_token("") },
                                                  parameter_declaration{int_type, make_id_token("") } } },
                            builtin_function_kind::string_erase_pos_count
                    },
                    builtin_function_info{ // bool starts_with(string str)
                            bool_type,
                            function_signature{ make_id_token("starts_with"),
                                                { parameter_declaration{string_type, make_id_token("") } } },
                            builtin_function_kind::string_starts_with
                    },
                    builtin_function_info{ // bool ends_with(string str)
                            bool_type,
                            function_signature{ make_id_token("ends_with"),
                                                { parameter_declaration{string_type, make_id_token("") } } },
                            builtin_function_kind::string_ends_with
                    },
                    builtin_function_info{ // string& replace(int pos, int count, string str)
                            string_type, // Todo: reference
                            function_signature{ make_id_token("replace"),
                                                { parameter_declaration{int_type, make_id_token("") },
                                                  parameter_declaration{int_type, make_id_token("") },
                                                  parameter_declaration{string_type, make_id_token("") } } },
                            builtin_function_kind::string_replace_pos_count_str
                    },
                    builtin_function_info{ // string substr(int pos)
                            string_type,
                            function_signature{ make_id_token("substr"),
                                                { parameter_declaration{int_type, make_id_token("") } } },
                            builtin_function_kind::string_substr_pos
                    },
                    builtin_function_info{ // string substr(int pos, int count)
                            string_type,
                            function_signature{ make_id_token("substr"),
                                                { parameter_declaration{int_type, make_id_token("") },
                                                  parameter_declaration{int_type, make_id_token("") } } },
                            builtin_function_kind::string_substr_pos_count
                    },
                    builtin_function_info{ // void resize(int new_size)
                            void_type,
                            function_signature{ make_id_token("resize"),
                                                { parameter_declaration{int_type, make_id_token("") } } },
                            builtin_function_kind::string_resize_newsize
                    },
                    builtin_function_info{ // void resize(int new_size, string fill)
                            void_type,
                            function_signature{ make_id_token("resize"),
                                                { parameter_declaration{int_type, make_id_token("") },
                                                  parameter_declaration{string_type, make_id_token("") } } },
                            builtin_function_kind::string_resize_newsize_fill
                    },
                    builtin_function_info{ // int find(string str)
                            int_type,
                            function_signature{ make_id_token("find"),
                                                { parameter_declaration{string_type, make_id_token("") } } },
                            builtin_function_kind::string_find_str
                    },
                    builtin_function_info{ // int find(string str, int pos)
                            int_type,
                            function_signature{ make_id_token("find"),
                                                { parameter_declaration{string_type, make_id_token("") },
                                                  parameter_declaration{int_type, make_id_token("") } } },
                            builtin_function_kind::string_find_str_pos
                    },
                    builtin_function_info{ // int find_not_of(string str)
                            int_type,
                            function_signature{ make_id_token("find_not_of"),
                                                { parameter_declaration{string_type, make_id_token("") } } },
                            builtin_function_kind::string_find_not_of_str
                    },
                    builtin_function_info{ // int find_not_of(string str, int pos)
                            int_type,
                            function_signature{ make_id_token("find_not_of"),
                                                { parameter_declaration{string_type, make_id_token("") },
                                                  parameter_declaration{int_type, make_id_token("") } } },
                            builtin_function_kind::string_find_not_of_str_pos
                    },
                    builtin_function_info{ // int find_last(string str)
                            int_type,
                            function_signature{ make_id_token("find_last"),
                                                { parameter_declaration{string_type, make_id_token("") } } },
                            builtin_function_kind::string_find_last_str
                    },
                    builtin_function_info{ // int find_last_not_of(string str)
                            int_type,
                            function_signature{ make_id_token("find_last_not_of"),
                                                { parameter_declaration{string_type, make_id_token("") } } },
                            builtin_function_kind::string_find_last_not_of_str
                    },
                    builtin_function_info{ // bool contains(string str)
                            bool_type,
                            function_signature{ make_id_token("contains"),
                                                { parameter_declaration{string_type, make_id_token("") } } },
                            builtin_function_kind::string_contains
                    },
                    builtin_function_info{ // void lower()
                            void_type,
                            function_signature{ make_id_token("lower"), {} },
                            builtin_function_kind::string_lower
                    },
                    builtin_function_info{ // string make_lower()
                            string_type,
                            function_signature{ make_id_token("make_lower"), {} },
                            builtin_function_kind::string_make_lower
                    },
                    builtin_function_info{ // void upper()
                            void_type,
                            function_signature{ make_id_token("upper"), {} },
                            builtin_function_kind::string_upper
                    },
                    builtin_function_info{ // string make_upper()
                            string_type,
                            function_signature{ make_id_token("make_upper"), {} },
                            builtin_function_kind::string_make_upper
                    }

            };

            add_type_member_functions(string_manipulator, functions);
        }

        type_builder builtin_sema_context::add_version_type()
        {
            static const auto token = make_token(token_type_t::kw_version, "version");
            static const auto name_representation = ast::type_representation{ token };
            type_builder builder{ m_type_factory, m_function_factory, m_context_factory, *this, name_representation };
            builder.build_and_register_in_context();
            return builder;
        }

        void builtin_sema_context::add_version_member_functions(type_builder &string_manipulator)
        {
            const auto types_finder = builtin_types_finder{ *this };
            const auto& int_type = types_finder.find_int();
            const auto& bool_type = types_finder.find_bool();
            const auto& string_type = types_finder.find_string();
            const auto& version_type = types_finder.find_version();

            const auto functions = {
                    builtin_function_info{ // version(int major)
                            version_type,
                            function_signature{ make_id_token("version"),
                                                { parameter_declaration{int_type, make_id_token("") } } },
                            builtin_function_kind::version_ctor_major
                    },
                    builtin_function_info{ // version(int major, int minor)
                            version_type,
                            function_signature{ make_id_token("version"),
                                                { parameter_declaration{int_type, make_id_token("") },
                                                  parameter_declaration{int_type, make_id_token("") }} },
                            builtin_function_kind::version_ctor_major_minor
                    },
                    builtin_function_info{ // version(int major, int minor, int patch)
                            version_type,
                            function_signature{ make_id_token("version"),
                                                { parameter_declaration{int_type, make_id_token("") },
                                                  parameter_declaration{int_type, make_id_token("") },
                                                  parameter_declaration{int_type, make_id_token("") } } },
                            builtin_function_kind::version_ctor_major_minor_patch
                    },
                    builtin_function_info{ // version(int major, int minor, int patch, int tweak)
                            version_type,
                            function_signature{ make_id_token("version"),
                                                { parameter_declaration{int_type, make_id_token("") },
                                                  parameter_declaration{int_type, make_id_token("") },
                                                  parameter_declaration{int_type, make_id_token("") },
                                                  parameter_declaration{int_type, make_id_token("") }} },
                            builtin_function_kind::version_ctor_major_minor_patch_tweak
                    },
                    builtin_function_info{ // bool operator==(version)
                            bool_type,
                            function_signature{ make_id_token("=="),
                                                { parameter_declaration{version_type, make_id_token("") } } },
                            builtin_function_kind::version_operator_equal_equal
                    },
                    builtin_function_info{ // bool operator!=(version)
                            bool_type,
                            function_signature{ make_id_token("!="),
                                                { parameter_declaration{version_type, make_id_token("") } } },
                            builtin_function_kind::version_operator_not_equal
                    },
                    builtin_function_info{ // bool operator<(version)
                            bool_type,
                            function_signature{ make_id_token("<"),
                                                { parameter_declaration{version_type, make_id_token("") } } },
                            builtin_function_kind::version_operator_less
                    },
                    builtin_function_info{ // bool operator<=(version)
                            bool_type,
                            function_signature{ make_id_token("<="),
                                                { parameter_declaration{version_type, make_id_token("") } } },
                            builtin_function_kind::version_operator_less_equal
                    },
                    builtin_function_info{ // bool operator>(version)
                            bool_type,
                            function_signature{ make_id_token(">"),
                                                { parameter_declaration{version_type, make_id_token("") } } },
                            builtin_function_kind::version_operator_greater
                    },
                    builtin_function_info{ // bool operator>=(version)
                            bool_type,
                            function_signature{ make_id_token(">="),
                                                { parameter_declaration{version_type, make_id_token("") } } },
                            builtin_function_kind::version_operator_greater_equal
                    },
                    builtin_function_info{ // int major()
                            int_type,
                            function_signature{ make_id_token("major"), {} },
                            builtin_function_kind::version_major
                    },
                    builtin_function_info{ // int minor()
                            int_type,
                            function_signature{ make_id_token("minor"), {} },
                            builtin_function_kind::version_minor
                    },
                    builtin_function_info{ // int patch()
                            int_type,
                            function_signature{ make_id_token("patch"), {} },
                            builtin_function_kind::version_patch
                    },
                    builtin_function_info{ // int tweak()
                            int_type,
                            function_signature{ make_id_token("tweak"), {} },
                            builtin_function_kind::version_tweak
                    },
                    builtin_function_info{ // string to_string()
                            string_type,
                            function_signature{ make_id_token("to_string"), {} },
                            builtin_function_kind::version_to_string
                    }
            };

            add_type_member_functions(string_manipulator, functions);
        }

        type_builder builtin_sema_context::add_project_type()
        {
            static const auto token = make_token(token_type_t::kw_version, "project");
            static const auto name_representation = ast::type_representation{ token };
            type_builder builder{ m_type_factory, m_function_factory, m_context_factory, *this, name_representation };
            builder.build_and_register_in_context();
            return builder;
        }

        void builtin_sema_context::add_project_member_functions(type_builder &project_manipulator)
        {
            const auto types_finder = builtin_types_finder{ *this };
            const auto& int_type = types_finder.find_int();
            const auto& bool_type = types_finder.find_bool();
            const auto& string_type = types_finder.find_string();
            const auto& version_type = types_finder.find_version();
            const auto& project_type = types_finder.find_project();

            const auto string_token = make_token( token_type_t::kw_string, "string" );
            const auto string_type_representation = ast::type_representation{ string_token };
            const auto sources_list_type_name_representation = ast::type_representation{
                    { make_token( token_type_t::kw_list, "string" ),
                      make_token( token_type_t::less, "<" ),
                      string_token,
                      make_token( token_type_t::greater, ">" ) },
                    { string_type_representation }
            };
            auto factory = sema_generic_type_factory{ *this,
                                                      *this,
                                                      m_type_factory,
                                                      m_function_factory,
                                                      m_context_factory };
            const auto& sources_type = *factory.create_generic(sources_list_type_name_representation);

            const auto functions = {
                    builtin_function_info{ // project(string name)
                            project_type,
                            function_signature{make_id_token("project"),
                                               {parameter_declaration{string_type, make_id_token("")}}},
                            builtin_function_kind::project_ctor_name
                    },
                    builtin_function_info{ // string name()
                            project_type,
                            function_signature{make_id_token("name"), {}},
                            builtin_function_kind::project_ctor_name
                    },
                    builtin_function_info{ // string name()
                            project_type,
                            function_signature{make_id_token("add_executable"),
                                               {parameter_declaration{string_type, make_id_token("")},
                                                parameter_declaration{sources_type, make_id_token("")}} },
                            builtin_function_kind::project_add_executable
                    }
            };

            add_type_member_functions(project_manipulator, functions);
        }
    }
}
