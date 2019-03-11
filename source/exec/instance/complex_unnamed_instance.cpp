#include "exec/instance/complex_unnamed_instance.hpp"

#include "ast/class_type.hpp"
#include "common/assert.hpp"
#include "exec/instance/instance_factory.hpp"
#include "sema/sema_type.hpp"
#include "complex_unnamed_instance.hpp"


namespace cmsl
{
    namespace exec
    {
        namespace inst
        {
            complex_unnamed_instance::complex_unnamed_instance(const ast::type &type)
                    : m_kind{ type.is_builtin() ? kind::builtin : kind::user }
                    , m_type{ &type }
                    , m_members{ get_init_data() }
            {}

            complex_unnamed_instance::complex_unnamed_instance(kind k, const ast::type &type)
                    : m_kind{ k }
                    , m_type{ &type }
                    , m_members{ get_init_data() }
            {}

            complex_unnamed_instance::complex_unnamed_instance(const ast::type &type, instance_members_t members)
                    : m_kind{ kind::user }
                    , m_type{ &type }
                    , m_members{ get_init_data(std::move(members)) }
            {}

            complex_unnamed_instance::complex_unnamed_instance(const sema::sema_type &type)
                : m_sema_type{ &type }
                , m_members{ get_init_data() }
            {}

            complex_unnamed_instance::complex_unnamed_instance(const sema::sema_type &type, instance_members_t members)
                    : m_sema_type{ &type }
                    , m_members{ std::move(members) }
            {}

            instance_members_t complex_unnamed_instance::get_init_data() const
            {
                return m_sema_type ? create_init_members_sema() : create_init_members();
            }

            instance_members_t complex_unnamed_instance::get_init_data(instance_members_t members) const
            {
                return std::move(members);
            }

            instance_value_t complex_unnamed_instance::get_value() const
            {
                CMSL_UNREACHABLE("Getting value of complex type");
            }

            instance_value_t& complex_unnamed_instance::get_value_ref()
            {
                CMSL_UNREACHABLE("Getting value of complex type");
            }

            const instance_value_t &complex_unnamed_instance::get_value_cref() const
            {
                CMSL_UNREACHABLE("Getting value of complex type");
            }

            void complex_unnamed_instance::assign(instance_value_t val)
            {
                // TODO

                CMSL_UNREACHABLE("Not implemented yet");
            }

            instance *complex_unnamed_instance::get_member(cmsl::string_view name)
            {
                auto found = m_members.find(name);

                return found != std::end(m_members)
                       ? found->second.get()
                       : nullptr;
            }

            const instance *complex_unnamed_instance::get_cmember(cmsl::string_view name) const
            {
                auto found = m_members.find(name);

                return found != std::end(m_members)
                       ? found->second.get()
                       : nullptr;
            }

            bool complex_unnamed_instance::is_fundamental() const
            {
                return m_kind == kind::builtin;
            }

            std::unique_ptr<instance> complex_unnamed_instance::copy() const
            {
                return m_sema_type
                       ? std::make_unique<complex_unnamed_instance>(*m_sema_type, copy_members())
                       : std::make_unique<complex_unnamed_instance>(*m_type, copy_members());
            }

            instance_members_t complex_unnamed_instance::copy_members() const
            {
                instance_members_t m;

                std::transform(std::cbegin(m_members), std::cend(m_members),
                               std::inserter(m, m.end()),
                               [](const auto &member_pair)
                               {
                                   return std::make_pair(member_pair.first,
                                                         member_pair.second->copy());
                               });

                return std::move(m);
            }

            instance_members_t complex_unnamed_instance::create_init_members() const
            {
                instance_members_t members;

                const auto& class_type = dynamic_cast<const ast::class_type&>(*m_type);
                const auto& members_decl = class_type.get_members_decl();

                std::transform(std::cbegin(members_decl), std::cend(members_decl),
                               std::inserter(members, std::end(members)),
                               [this](const auto& member_decl)
                               {
                                   const auto& member_type = *member_decl.ty;

                                   auto member_inst = instance_factory{}.create(*member_decl.ty);
                                   return std::make_pair(member_decl.name, std::move(member_inst));
                               });

                return members;
            }

            bool complex_unnamed_instance::has_function(cmsl::string_view name) const
            {
                return m_type->has_function(name);
            }

            const ast::function* complex_unnamed_instance::get_function(cmsl::string_view name) const
            {
                return m_type->get_function(name);
            }

            const ast::type& complex_unnamed_instance::get_type() const
            {
                return *m_type;
            }

            bool complex_unnamed_instance::is_ctor(cmsl::string_view name) const
            {
                return get_type().get_name() == name;
            }

            sema::single_scope_function_lookup_result_t complex_unnamed_instance::get_sema_function(lexer::token::token name) const
            {
                return m_sema_type->find_member_function(name);
            }

            const sema::sema_type &complex_unnamed_instance::get_sema_type() const
            {
                return *m_sema_type;
            }

            instance_members_t complex_unnamed_instance::create_init_members_sema() const
            {
                instance_members_t members;

                const auto& member_declarations = m_sema_type->members();

                std::transform(std::cbegin(member_declarations), std::cend(member_declarations),
                               std::inserter(members, std::end(members)),
                               [this](const auto& member_decl)
                               {
                                   auto member_inst = instance_factory2{}.create(member_decl.ty);
                                   return std::make_pair(member_decl.name.str(), std::move(member_inst));
                               });

                return members;
            }

        }
    }
}
