#include "exec/instance/simple_unnamed_instance.hpp"

#include "common/assert.hpp"
#include "exec/instance/instance_value_variant.hpp"
#include "sema/sema_type.hpp"

namespace cmsl::exec::inst
{
            simple_unnamed_instance::simple_unnamed_instance(const sema::sema_type &type)
                    : m_sema_type{ &type }
                    , m_data{ get_init_data() }
            {}

            simple_unnamed_instance::simple_unnamed_instance(const sema::sema_type &type, instance_value_variant value)
                    : m_sema_type{ &type }
                    , m_data{ std::move(value) }
            {}

    instance_value_variant simple_unnamed_instance::get_init_data() const
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
                    return list_value{};
                }
                else if(name == "project")
                {
                    return project_value{""};
                }

                CMSL_UNREACHABLE("Unknown type");
            }

    instance_value_variant simple_unnamed_instance::get_init_data(instance_value_variant val) const
            {
                return val;
            }

    instance_value_variant simple_unnamed_instance::get_value() const
            {
                return m_data;
            }

    instance_value_variant &simple_unnamed_instance::get_value_ref()
            {
                return m_data;
            }

            const instance_value_variant& simple_unnamed_instance::get_value_cref() const
            {
                return m_data;
            }

            void simple_unnamed_instance::assign(instance_value_variant val)
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
                return std::make_unique<simple_unnamed_instance>(*m_sema_type, get_value());
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
