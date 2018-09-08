#include "exec/instance/instance_converter.hpp"

#include "ast/common_type_finder.hpp"
#include "ast/type.hpp"
#include "exec/instance/conversion_visitor.hpp"
#include "exec/instance/instances_holder.hpp"
#include "exec/instance/instance.hpp"


namespace cmsl
{
    namespace exec
    {
        namespace inst
        {
            instance_converter::instance_converter(instances_holder &instances)
                : m_instances{ instances }
            {}

            std::pair<instance*, instance*> instance_converter::prepare_for_operator(instance *lhs, instance *rhs)
            {
                const auto& lhs_type = lhs->get_type();
                const auto& rhs_type = rhs->get_type();

                if(lhs_type == rhs_type)
                {
                    // No conversion needed
                    return { lhs, rhs };
                }

                const auto common_type = ast::common_type_finder{}.find_common_type(lhs_type, rhs_type);

                if(common_type == nullptr)
                {
                    return { nullptr, nullptr };
                }

                if(*common_type != lhs_type)
                {
                    // Need to convert lhs to rhs' type

                    // todo get rid of these when we'll start using std::varian
                    auto lhs_val = lhs->get_value();
                    auto rhs_val = rhs->get_value();
                    const auto converted_value = conversion_visitor{}.visit(lhs_val, rhs_val);
                    auto converted_lhs = m_instances.create(converted_value);
                    return { converted_lhs, rhs };
                }
                else
                {
                    // Need to convert rhs to lhs' type

                    // todo get rid of these when we'll start using std::varian
                    auto lhs_val = lhs->get_value();
                    auto rhs_val = rhs->get_value();
                    const auto converted_value = conversion_visitor{}.visit(rhs_val, lhs_val);
                    auto converted_rhs = m_instances.create(converted_value);
                    return { lhs, converted_rhs };
                }
            }

            instance *instance_converter::convert_to_type(instance *from, const ast::type &desired_type)
            {
                return convert_to_type(from->get_value(), desired_type);
            }

            instance *instance_converter::convert_to_type(instance_value_t from, const ast::type &desired_type)
            {
                auto result_instance = m_instances.create(desired_type);
                auto result_val = result_instance->get_value();
                const auto converted_value = conversion_visitor{}.visit(from, result_val);
                result_instance->assign(converted_value);
                return result_instance;
            }
        }
    }
}

