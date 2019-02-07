#pragma once

#include "common/assert.hpp"
#include "common/type_traits.hpp"
#include "exec/instance/instance.hpp"
#include "exec/instance/instance_value.hpp"

namespace cmsl
{
    namespace exec
    {
        struct size_visitor
        {
            inst::int_t visit(const inst::instance_value_t& inst_val) const
            {
                const auto value_type = static_cast<inst::instance_value_type>(inst_val.which());

                switch(value_type)
                {

                    case inst::instance_value_type::string:
                    {
                        return static_cast<inst::int_t>(inst_val.get_string_cref().size());
                    }

                    case inst::instance_value_type::generic:
                    {
                    const auto& generic_val = inst_val.get_generic_cref();
                        return generic_val.apply_const([](const auto& stored_value)
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
                        return inst_val.get_string_cref().empty();
                    }

                    case inst::instance_value_type::generic:
                    {
                        const auto& generic_val = inst_val.get_generic_cref();
                        return generic_val.apply_const([](const auto& stored_value)
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
                    case inst::instance_value_type::generic:
                    {
                        auto& generic_val = inst_val.get_generic_ref();
                        generic_val.apply([this](auto& stored_value)
                                          {
                                              stored_value.push_back(m_parameter.copy());
                                              return true;
                                          });
                    }
                    break;


                    default:
                        CMSL_UNREACHABLE("Called push_back() on type that does not support it");
                }
            }

        private:
            const inst::instance& m_parameter;
        };
    }
}
