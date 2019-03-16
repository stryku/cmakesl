#include "exec/instance/list_value.hpp"

#include "exec/instance/instance.hpp"

#include <algorithm>

namespace cmsl
{
    namespace exec
    {
        namespace inst
        {
            list_value::list_value(const list_value &other)
            {
                if(this == &other)
                {
                    return;
                }

                m_list.reserve(other.m_list.size());

                std::transform(std::cbegin(other.m_list), std::cend(other.m_list),
                        std::back_inserter(m_list),
                        [](const auto& element)
                               {
                                   return element->copy();
                               });
            }
        }
    }
}
