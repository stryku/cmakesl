#include "exec/instance.hpp"

#include <boost/assert.hpp>

namespace cmsl
{
    namespace exec
    {
        instance::instance(kind k, const ast::type& type)
            : m_kind{ k }
            , m_type{ type }
            , m_data{ get_init_data() }
        {}

        instance::instance(const ast::type& type, int value)
            : m_kind{ kind::fundamental }
            , m_type{ type }
            , m_data{ get_init_data() }
        {}

        instance::instance(const ast::type& type, string_view_map<std::unique_ptr<instance>> m_members)
            : m_kind{ kind::user }
            , m_type{ type }
            , m_data{ get_init_data() }
        {}

        instance::data_t instance::get_init_data() const
        {
            switch (m_kind)
            {
                case kind::fundamental: return int{};
                case kind::user: return members_t{};
            }

            return {};
        }

        int instance::get_value() const
        {
            expect_fundamental();
            return boost::get<value_t>(m_data);
        }

        void instance::assign(int val)
        {
            expect_fundamental();
            boost::get<value_t>(m_data) = val;
        }

        void instance::expect_fundamental() const
        {
            BOOST_ASSERT_MSG(is_fundamental(), "Expected fundamental type");
        }

        void instance::expect_user() const
        {
            BOOST_ASSERT_MSG(!is_fundamental(), "Expected user type");
        }


        instance* instance::get_member(cmsl::string_view name)
        {
            expect_user();
            auto& members = boost::get<members_t>(m_data);

            auto found = members.find(name);

            return found != std::end(members)
                ? found->second.get()
                : nullptr;
        }

        bool instance::is_fundamental() const
        {
            return m_kind == kind::fundamental;
        }
    }
}
