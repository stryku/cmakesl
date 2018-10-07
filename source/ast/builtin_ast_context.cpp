#include "ast/builtin_ast_context.hpp"

#include "ast/type.hpp"
#include "ast/type_kind.hpp"
#include "ast/class_builder.hpp"
#include "ast/class_type.hpp"
#include "ast/builtin_function.hpp"

namespace cmsl
{
    namespace ast
    {
        builtin_ast_context::builtin_ast_context()
            : ast_context_impl{ nullptr }
        {
            add_types();
            add_functions();
        }

        void builtin_ast_context::add_types()
        {
            const auto builtin_types_kind = {
                    std::make_pair("int", type_kind::k_int),
                    std::make_pair("double", type_kind::k_double),
                    std::make_pair("bool", type_kind::k_bool)
            };

            for (const auto pair : builtin_types_kind)
            {
                add_type(std::make_unique<type>(pair.first, pair.second, nullptr));
            }

            add_string_type();
            add_version_type();
            add_project_type();
        }

        void builtin_ast_context::add_string_type()
        {
            class_builder builder{ *this, "string" };

            const auto string_functions = {
                builtin_function_kind::size,
                builtin_function_kind::empty
            };

            for(const auto fun_kind : string_functions)
            {
                auto fun = std::make_unique<builtin_function>(fun_kind, builtin_function::params_declaration_t{});
                builder.with_function(std::move(fun));
            }

            auto string_class_node = builder.build();
            auto string_type = std::make_unique<class_type>(std::move(string_class_node), type_kind::k_string);

            add_type(std::move(string_type));
        }

        void builtin_ast_context::add_version_type()
        {
            class_builder builder{ *this, "version" };

            const auto int_type = find_type("int");

            const auto member_names = {
                "major",
                "minor",
                "patch",
                "tweak"
            };

            for(const auto member_name : member_names)
            {
                auto member = member_declaration{int_type, member_name};
                builder.with_member(std::move(member));
            }

            builtin_function::params_declaration_t params{
                    parameter_declaration{ int_type }, // major
                    parameter_declaration{ int_type }, // minor
                    parameter_declaration{ int_type }, // patch
                    parameter_declaration{ int_type }  // tweak
            };

            auto fun = std::make_unique<builtin_function>(builtin_function_kind::version_ctor,
                                                              std::move(params));

            builder.with_function(std::move(fun));

            auto node = builder.build();
            auto version_type = std::make_unique<class_type>(std::move(node), type_kind::k_version);

            add_type(std::move(version_type));
        }

        lexer::token::token builtin_ast_context::fake_name_token() const
        {
            return lexer::token::token{ lexer::token::token_type::identifier };
        }

        void builtin_ast_context::add_functions()
        {
            add_cmake_minimum_required();
        }

        void builtin_ast_context::add_cmake_minimum_required()
        {
            const auto version_type = find_type("version");

            builtin_function::params_declaration_t params{
                    parameter_declaration{ version_type } // version
            };

            auto fun = std::make_unique<builtin_function>(builtin_function_kind::cmake_minimum_required,
                                                              std::move(params));

            add_function(std::move(fun));
        }

        void builtin_ast_context::add_project_type()
        {
            class_builder builder{ *this, "project" };

            const auto string_type = find_type("string");

            builtin_function::params_declaration_t params{
                parameter_declaration{ string_type}, // name
            };

            auto fun = std::make_unique<builtin_function>(builtin_function_kind::project_ctor,
                                                          std::move(params));

            builder.with_function(std::move(fun));

            auto node = builder.build();
            auto project_type = std::make_unique<class_type>(std::move(node), type_kind::k_project);

            add_type(std::move(project_type));
        }
    }
}
