#include "exec/instance/simple_unnamed_instance.hpp"

#include "ast/class_type.hpp"
#include "common/assert.hpp"
#include "exec/instance/instance_value.hpp"
#include "sema/sema_type.hpp"

namespace cmsl
{
    namespace exec
    {
        namespace inst
        {
            simple_unnamed_instance::simple_unnamed_instance(const ast::type &type)
                    : m_kind{ type.is_builtin() ? kind::builtin : kind::user }
                    , m_type{ &type }
                    , m_data{ get_init_data() }
            {}

            simple_unnamed_instance::simple_unnamed_instance(kind k, const ast::type &type)
                    : m_kind{ k }
                    , m_type{ &type }
                    , m_data{ get_init_data() }
            {}

            simple_unnamed_instance::simple_unnamed_instance(const ast::type &type, instance_value_t value)
                    : m_kind{ kind::builtin }
                    , m_type{ &type }
                    , m_data{ get_init_data(value) }
            {}

            simple_unnamed_instance::simple_unnamed_instance(const sema::sema_type &type)
                    : m_sema_type{ &type }
                    , m_data{ get_init_data() }
            {}

            simple_unnamed_instance::simple_unnamed_instance(const sema::sema_type &type, instance_value_t value)
                    : m_sema_type{ &type }
                    , m_data{ std::move(value) }
            {}

            instance_value_t simple_unnamed_instance::get_init_data() const
            {
                if(m_type)
                {
                    switch (m_type->get_kind())
                    {
                        case ast::type_kind::k_bool:
                            return bool{};
                        case ast::type_kind::k_int:
                            return int_t{};
                        case ast::type_kind::k_double:
                            return double{};
                        case ast::type_kind::k_string:
                            return std::string{};
                        case ast::type_kind::k_version:
                            return version_value{ 0u };
                        case ast::type_kind::k_list:
                            return get_init_list_data();

                        default:
                            CMSL_UNREACHABLE("Unknown simple builtin type kind");
                    }
                }
                else
                {
                    const auto name = m_sema_type->name().primary_name().str();
                    // Todo: find better way than comparing strings.
                    if(name == "bool")
                    {
                        return false;
                    }
                    else if(name == "int")
                    {
                        return int_t{ 0 };
                    }
                    else if(name == "double")
                    {
                        return 0.0;
                    }
                    else if(name == "string")
                    {
                        return std::string{};
                    }
                    else if(name == "version")
                    {
                        return version_value{ 0u };
                    }
                    else if(name == "list")
                    {
                        return instance_value_t::list_t{};
                    }

                    CMSL_UNREACHABLE("Unknown type");
                }
            }

            instance_value_t simple_unnamed_instance::get_init_data(instance_value_t val) const
            {
                return val;
            }

            instance_value_t simple_unnamed_instance::get_value() const
            {
                return m_data;
            }

            instance_value_t &simple_unnamed_instance::get_value_ref()
            {
                return m_data;
            }

            const instance_value_t& simple_unnamed_instance::get_value_cref() const
            {
                return m_data;
            }

            void simple_unnamed_instance::assign(instance_value_t val)
            {
                m_data = val;
            }

            instance *simple_unnamed_instance::get_member(cmsl::string_view)
            {
                return nullptr;
            }

            const instance *simple_unnamed_instance::get_cmember(cmsl::string_view name) const
            {
                return nullptr;
            }

            std::unique_ptr<instance> simple_unnamed_instance::copy() const
            {
                return std::make_unique<simple_unnamed_instance>(*m_type, get_value());
            }

            bool simple_unnamed_instance::has_function(cmsl::string_view name) const
            {
                return m_type->has_function(name);
            }

            const ast::function* simple_unnamed_instance::get_function(cmsl::string_view name) const
            {
                return m_type->get_function(name);
            }

            const ast::type& simple_unnamed_instance::get_type() const
            {
                return *m_type;
            }

            instance_value_t simple_unnamed_instance::get_init_list_data() const
            {
                const auto k_list_type = generic_instance_value::generic_instance_value_type::list;
                return generic_instance_value{ get_type(), k_list_type };
            }

            bool simple_unnamed_instance::is_ctor(cmsl::string_view name) const
            {
                return get_type().get_name() == name;
            }

            sema::single_scope_function_lookup_result_t simple_unnamed_instance::get_sema_function(lexer::token::token name) const
            {
                return m_sema_type->find_member_function(name);
            }

            const sema::sema_type &simple_unnamed_instance::get_sema_type() const
            {
                return *m_sema_type;
            }
        }
    }
}
