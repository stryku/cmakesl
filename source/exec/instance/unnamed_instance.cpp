#include "exec/instance/unnamed_instance.hpp"

#include "common/assert.hpp"

namespace cmsl
{
    namespace exec
    {
        namespace inst
        {
            unnamed_instance::unnamed_instance(kind k, const ast::type &type)
                    : m_kind{k}
                    , m_type{type}
                    , m_data{get_init_data()}
            {}

            unnamed_instance::unnamed_instance(const ast::type &type, int value)
                    : m_kind{kind::fundamental}
                    , m_type{type}
                    , m_data{get_init_data(value)}
            {}

            unnamed_instance::unnamed_instance(const ast::type &type, members_t members)
                    : m_kind{kind::user}
                    , m_type{type}
                    , m_data{get_init_data(std::move(members))}
            {}

            unnamed_instance::data_t unnamed_instance::get_init_data() const
            {
                switch (m_kind)
                {
                    case kind::fundamental:
                        return int{};
                    case kind::user:
                        return members_t{};
                }

                CMSL_UNREACHABLE("Unknown unnamed_instance kind provided");
            }

            unnamed_instance::data_t unnamed_instance::get_init_data(int val) const
            {
                return val;
            }

            unnamed_instance::data_t unnamed_instance::get_init_data(members_t members) const
            {
                return std::move(members);
            }

            int unnamed_instance::get_value() const
            {
                expect_fundamental();
                return boost::get<value_t>(m_data);
            }

            void unnamed_instance::assign(int val)
            {
                expect_fundamental();
                boost::get<value_t>(m_data) = val;
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
                       ? found->second
                              .get()
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
        }
    }
}
