#include "exec/instance/unnamed_instance.hpp"

#include "ast/class_type.hpp"
#include "ast/type_kind.hpp"
#include "common/assert.hpp"

namespace cmsl
{
    namespace exec
    {
        namespace inst
        {
            unnamed_instance::unnamed_instance(const ast::type &type)
                : m_kind{ type.is_fundamental() ? kind::fundamental : kind::user }
                , m_type{ type }
                , m_data{ get_init_data() }
            {}

            unnamed_instance::unnamed_instance(kind k, const ast::type &type)
                : m_kind{ k }
                , m_type{ type }
                , m_data{ get_init_data() }
            {}

            unnamed_instance::unnamed_instance(const ast::type &type, instance_value_t value)
                : m_kind{ kind::fundamental }
                , m_type{ type }
                , m_data{ get_init_data(value) }
            {}

            unnamed_instance::unnamed_instance(const ast::type &type, members_t members)
                : m_kind{ kind::user }
                , m_type{ type }
                , m_data{ get_init_data(std::move(members)) }
            {}

            unnamed_instance::data_t unnamed_instance::get_init_data() const
            {
                switch (m_kind)
                {
                    case kind::fundamental:
                        return get_fundamental_init_data();
                    case kind::user:
                        return create_init_members();
                }

                CMSL_UNREACHABLE("Unknown unnamed_instance kind provided");
            }

            unnamed_instance::data_t unnamed_instance::get_init_data(instance_value_t val) const
            {
                return val;
            }

            unnamed_instance::data_t unnamed_instance::get_init_data(members_t members) const
            {
                return std::move(members);
            }

            instance_value_t unnamed_instance::get_value() const
            {
                expect_fundamental();
                return boost::get<instance_value_t>(m_data);
            }

            void unnamed_instance::assign(instance_value_t val)
            {
                expect_fundamental();
                boost::get<instance_value_t>(m_data) = val;
            }

            void unnamed_instance::expect_fundamental() const
            {
                BOOST_ASSERT_MSG(is_fundamental(), "Expected fundamental type");
            }

            void unnamed_instance::expect_user() const
            {
                BOOST_ASSERT_MSG(!is_fundamental(), "Expected user type");
            }


            instance *unnamed_instance::get_member(cmsl::string_view name)
            {
                expect_user();
                auto &members = boost::get<members_t>(m_data);

                auto found = members.find(name);

                return found != std::end(members)
                       ? found->second.get()
                       : nullptr;
            }

            bool unnamed_instance::is_fundamental() const
            {
                return m_kind == kind::fundamental;
            }

            std::unique_ptr<instance> unnamed_instance::copy() const
            {
                switch (m_kind)
                {
                    case kind::fundamental:
                        return std::make_unique<unnamed_instance>(m_type, get_value());
                    case kind::user:
                        return std::make_unique<unnamed_instance>(m_type, copy_members());
                }

                CMSL_UNREACHABLE("Cloning unnamed_instance with unknown kind");
            }

            unnamed_instance::data_t unnamed_instance::get_fundamental_init_data() const
            {
                switch(m_type.get_kind())
                {
                    case ast::type_kind::k_bool: return bool{};
                    case ast::type_kind::k_int: return int{};
                    case ast::type_kind::k_double: return double{};
                }

                CMSL_UNREACHABLE("Unknown fundamental type kind");
            }

            unnamed_instance::members_t unnamed_instance::copy_members() const
            {
                members_t m;
                const auto &members = boost::get<members_t>(m_data);

                std::transform(std::cbegin(members), std::cend(members),
                               std::inserter(m, m.end()),
                               [](const auto &member_pair)
                               {
                                   return std::make_pair(member_pair.first,
                                                         member_pair.second->copy());
                               });

                return std::move(m);
            }

            unnamed_instance::members_t unnamed_instance::create_init_members() const
            {
                expect_user();

                members_t members;

                const auto& class_type = dynamic_cast<const ast::class_type&>(m_type);
                const auto& members_decl = class_type.get_members_decl();

                std::transform(std::cbegin(members_decl), std::cend(members_decl),
                               std::inserter(members, std::end(members)),
                               [this](const auto& member_decl)
                               {
                                   auto member_inst = std::make_unique<unnamed_instance>(*member_decl.ty);
                                   return std::make_pair(member_decl.name.str(), std::move(member_inst));
                               });

                return members;
            }

            bool unnamed_instance::has_function(cmsl::string_view name) const
            {
                return m_type.has_function(name);
            }

            const ast::function_node* unnamed_instance::get_function(cmsl::string_view name) const
            {
                return m_type.get_function(name);
            }

            const ast::type& unnamed_instance::get_type() const
            {
                return m_type;
            }
        }
    }
}
