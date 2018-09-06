#pragma once

#include "common/type_traits.hpp"
#include "exec/instance/instance_value.hpp"

namespace cmsl
{
    namespace exec
    {
        struct size_visitor
        {
            int visit(const inst::instance_value_t& inst_val) const
            {
                const auto value_type = static_cast<inst::instance_value_type>(inst_val.which());

                switch(value_type)
                {

                    case inst::instance_value_type::string:
                    {
                        const auto& val = boost::get<std::string>(inst_val);
                        return static_cast<int>(val.size()); // todo introduce int alias
                    }

                    case inst::instance_value_type::generic:
                    {
                        auto& generic_val = boost::get<inst::generic_instance_value>(inst_val);
                        return generic_val.apply([](const auto& stored_value)
                                                 {
                                                     return stored_value.size();
                                                 });
                    }

                    case inst::instance_value_type::bool_:
                    case inst::instance_value_type::int_:
                    case inst::instance_value_type::double_:
                        CMSL_UNREACHABLE("Called size() on fundamental type");

                    default:
                        CMSL_UNREACHABLE("Called size() on unknown type");
                }
            }
        };

        struct empty_visitor
        {
            bool visit(const inst::instance_value_t& inst_val) const
            {
                const auto value_type = static_cast<inst::instance_value_type>(inst_val.which());

                switch(value_type)
                {
                    case inst::instance_value_type::string:
                    {
                        const auto& val = boost::get<std::string>(inst_val);
                        return val.empty();
                    }

                    case inst::instance_value_type::generic:
                    {
                        auto& generic_val = boost::get<inst::generic_instance_value>(inst_val);
                        return generic_val.apply([](const auto& stored_value)
                                                 {
                                                     return stored_value.empty();
                                                 });
                    }

                    case inst::instance_value_type::bool_:
                    case inst::instance_value_type::int_:
                    case inst::instance_value_type::double_:
                        CMSL_UNREACHABLE("Called empty() on fundamental type");

                    default:
                        CMSL_UNREACHABLE("Called empty() on unknown type");
                }
            }
        };

        struct push_back_visitor
        {
            explicit push_back_visitor(const inst::instance& parameter)
                : m_parameter{ parameter }
            {}

            void visit(inst::instance_value_t& inst_val) const
            {
                const auto value_type = static_cast<inst::instance_value_type>(inst_val.which());

                switch(value_type)
                {
                    case inst::instance_value_type::bool_:break;
                    case inst::instance_value_type::int_:break;
                    case inst::instance_value_type::double_:break;
                    case inst::instance_value_type::string:break;

                    case inst::instance_value_type::generic:
                    {
                        auto& generic_val = boost::get<inst::generic_instance_value>(inst_val);
                        generic_val.apply([this](auto& stored_value)
                                          {
                                              stored_value.push_back(m_parameter.copy());
                                              return true;
                                          });
                    }
                }
            }

        private:
            const inst::instance& m_parameter;
        };
    }
}
