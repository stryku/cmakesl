#include "exec/instance/instance_converter.hpp"

#include "ast/common_type_finder.hpp"
#include "ast/type.hpp"
#include "exec/instance/instances_holder.hpp"
#include "exec/instance/instance.hpp"

#include <type_traits>

namespace cmsl
{
    namespace exec
    {
        namespace inst
        {
            namespace details
            {
                struct convert_visitor : boost::static_visitor<instance_value_t>
                {
                    template<typename ValueToConvert,
                             typename DesiredType,
                             typename DecayedDesiredType = std::decay_t<ValueToConvert>,
                             typename = std::enable_if_t<std::is_arithmetic<DecayedDesiredType>::value,
                                                         void>>
                    instance_value_t operator()(ValueToConvert &&val, DesiredType &&) const
                    {
                        return static_cast<DecayedDesiredType>(val);
                    }
                };
            }

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
                    const auto converted_value = boost::apply_visitor(details::convert_visitor{}, lhs_val, rhs_val);
                    auto converted_lhs = m_instances.create(converted_value);
                    return { converted_lhs, rhs };
                }
                else
                {
                    // Need to convert rhs to lhs' type

                    // todo get rid of these when we'll start using std::varian
                    auto lhs_val = lhs->get_value();
                    auto rhs_val = rhs->get_value();
                    const auto converted_value = boost::apply_visitor(details::convert_visitor{}, rhs_val, lhs_val);
                    auto converted_rhs = m_instances.create(converted_value);
                    return { lhs, converted_rhs };
                }
            }
        }
    }
}

