#include "exec/builtin/builtin_member_functions.hpp"
#include "exec/instance/instance_value.hpp"
#include "exec/instance/instance.hpp"
#include "exec/instance/instances_holder.hpp"

#include "common/assert.hpp"

namespace cmsl
{
    namespace exec
    {
        namespace builtin
        {

            size_member_function::size_member_function(inst::instance &class_instance, inst::instances_holder_interface &instances)
                    : evaluatable_member_function(class_instance, instances)
            {}

            inst::instance *size_member_function::evaluate()
            {
                const auto got_size = get_size();
                return m_instances.create(static_cast<inst::int_t>(got_size));
            }

            size_t size_member_function::get_size()
            {
                const auto &value = m_class_instance.get_value_cref();
                const auto value_type = inst::get_inst_val_type(value);

                switch (value_type)
                {
                    case inst::instance_value_type::string:
                    {
                        const auto &string = boost::get<std::string>(value);
                        return string.size();
                    }

                    case inst::instance_value_type::generic:
                    {
                        auto &generic_val = boost::get<inst::generic_instance_value>(value);
                        return generic_val.apply_const([](const auto &stored_value)
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

                return static_cast<size_t>(-1);
            }

            empty_member_function::empty_member_function(inst::instance &class_instance, inst::instances_holder_interface &instances)
                    : evaluatable_member_function(class_instance, instances)
            {}

            inst::instance *empty_member_function::evaluate()
            {
                const auto is_empty = get_empty();
                return m_instances.create(is_empty);
            }

            bool empty_member_function::get_empty()
            {
                const auto &value = m_class_instance.get_value_cref();
                const auto value_type = inst::get_inst_val_type(value);

                switch (value_type)
                {
                    case inst::instance_value_type::string:
                    {
                        const auto &val = boost::get<std::string>(value);
                        return val.empty();
                    }

                    case inst::instance_value_type::generic:
                    {
                        auto &generic_val = boost::get<inst::generic_instance_value>(value);
                        return generic_val.apply_const([](const auto &stored_value)
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

                return true;
            }

            push_back_member_function::push_back_member_function(inst::instance &class_instance, inst::instances_holder_interface &instances, const inst::instance &parameter)
                    : evaluatable_member_function(class_instance, instances)
                    , m_parameter{parameter}
            {}

            inst::instance *push_back_member_function::evaluate()
            {
                auto &value = m_class_instance.get_value_ref();
                const auto value_type = inst::get_inst_val_type(value);

                switch (value_type)
                {
                    case inst::instance_value_type::generic: // todo handle generic types that doesn't support push_back, e.g. optional
                    {
                        auto &generic_val = boost::get<inst::generic_instance_value>(value);
                        const auto& parameter = m_parameter;
                        generic_val.apply([&parameter](auto &stored_value)
                                          {
                                              stored_value.push_back(parameter.copy());
                                              return true;
                                          });
                    }
                        break;

                    default:
                        CMSL_UNREACHABLE("Called push_back() on type that does not support it");
                }

                return nullptr;
            }
        }
    }
}
