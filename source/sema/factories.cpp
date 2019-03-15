#include "sema/factories.hpp"
#include "sema/builtin_sema_function.hpp"
#include "sema/user_sema_function.hpp"
#include "sema/sema_type.hpp"
#include "sema/sema_context.hpp"
#include "sema/type_builder.hpp"
#include "sema/builtin_function_kind.hpp"
#include "sema/builtin_types_finder.hpp"

// Todo: Move to common place.
namespace
{
    template<unsigned N>
    cmsl::lexer::token::token make_token(cmsl::lexer::token::token_type token_type, const char (&tok)[N])
    {
        // N counts also '\0'
        const auto src_range = cmsl::source_range{
                cmsl::source_location{ 1u, 1u, 0u },
                cmsl::source_location{ 1u, N, N - 1u }
        };
        return cmsl::lexer::token::token{ token_type, src_range, tok };
    }

    template<unsigned N>
    cmsl::lexer::token::token make_id_token(const char (&tok)[N])
    {
        return make_token(cmsl::lexer::token::token_type ::identifier, tok);
    }
}

namespace cmsl
{
    namespace sema
    {
        user_sema_function&
        sema_function_factory::create_user(const sema_context_interface &ctx, const sema_type &return_type, function_signature s)
        {
            return create_impl<user_sema_function>(ctx, return_type, std::move(s));
        }
        builtin_sema_function&
        sema_function_factory::create_builtin(const sema_context_interface &ctx, const sema_type &return_type, function_signature s, builtin_function_kind kind)
        {
            return create_impl<builtin_sema_function>(ctx, return_type, std::move(s), kind);
        }

        sema_context& sema_context_factory::create(const sema_context_interface *parent)
        {
            return create_impl<sema_context>(parent, sema_context_interface::context_type::namespace_);
        }

        sema_context &sema_context_factory::create_class(const sema_context_interface *parent)
        {
            return create_impl<sema_context>(parent, sema_context_interface::context_type::class_);
        }

        const sema_type&
        sema_type_factory::create(const sema_context_interface &ctx, ast::type_representation name, std::vector<member_info> members)
        {
            return create_impl<sema_type>(ctx, name, std::move(members));
        }

        sema_generic_type_factory::sema_generic_type_factory(sema_context_interface& generic_types_context,
                                                             const sema_context_interface& creation_context,
                                                             sema_type_factory& type_factory,
                                                             sema_function_factory& function_factory,
                                                             sema_context_factory& context_factory)
            : m_generic_types_context{ generic_types_context }
            , m_creation_context{ creation_context }
            , m_type_factory{ type_factory }
            , m_function_factory{ function_factory }
            , m_context_factory{ context_factory }
        {}

        const sema_type *
        sema_generic_type_factory::create_generic(const ast::type_representation& name)
        {
            const auto primary_name = name.primary_name();
            if(primary_name.str() == "list")
            {
                return create_list(name);
            }

            // Todo: report error 'primary_name is not a generic type'.
            return nullptr;
        }

        const sema_type *
        sema_generic_type_factory::create_list(const ast::type_representation& name)
        {
            const auto& nested_types = name.nested_types();
            if(nested_types.empty())
            {
                // Todo: Not enough parameters
                return nullptr;
            }
            constexpr auto list_generic_parameters{ 1u };
            if(nested_types.size() > list_generic_parameters)
            {
                // Todo: Too many parameters
                return nullptr;
            }

            const auto& value_type_representation = name.nested_types().front();
            const auto value_type = try_get_or_create_value_type(value_type_representation);
            if(!value_type)
            {
                return nullptr;
            }

            type_builder builder{ m_type_factory,
                                  m_function_factory,
                                  m_context_factory,
                                  m_generic_types_context,
                                  name };
            builder.build_and_register_in_context();

            // At this point we know that list type is created and registered in context. We can safely dereference it.
            const auto& list_type = *m_generic_types_context.find_type(name);
            builtin_types_finder finder{ m_creation_context };
            const auto& int_type = finder.find_int();
            const auto& void_type = finder.find_void();

            auto functions = {
                    type_builder::builtin_function_info{ // list()
                            list_type,
                            function_signature{ make_id_token("list"), {} },
                            builtin_function_kind::list_ctor
                    },
                    type_builder::builtin_function_info{ // int size()
                            int_type,
                            function_signature{ make_id_token("size"), {} },
                            builtin_function_kind::list_size
                    },
                    type_builder::builtin_function_info{ // list& operator+=(value_type)
                            list_type,
                            function_signature{ make_id_token("+="),
                                                { parameter_declaration{*value_type, make_id_token("") } } },
                            builtin_function_kind::list_operator_plus_equal_value
                    },
                    type_builder::builtin_function_info{ // list& operator+=(list)
                            list_type,
                            function_signature{ make_id_token("+="),
                                                { parameter_declaration{list_type, make_id_token("") } } },
                            builtin_function_kind::list_operator_plus_equal_list
                    },
                    type_builder::builtin_function_info{ // value_type& at(int)
                            *value_type,
                            function_signature{ make_id_token("at"),
                                                { parameter_declaration{int_type, make_id_token("") } } },
                            builtin_function_kind::list_at
                    }
            };

            builder.with_builtin_functions(functions);

            return &list_type;
        }

        const sema_type *
        sema_generic_type_factory::try_get_or_create_value_type(const ast::type_representation &name)
        {
            const auto found = m_creation_context.find_type(name);
            if(!name.is_generic())
            {
                if(!found)
                {
                    // Todo: Value type not found.
                    return nullptr;
                }
                return found;
            }

            if(!found)
            {
                // If it's a generic type and it's not found, try create it and return result.
                return create_generic(name);
            }

            return found;
        }
    }
}
