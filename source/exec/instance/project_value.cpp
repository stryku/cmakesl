#include "exec/instance/project_value.hpp"
#include "exec/instance/list_value.hpp"
#include "exec/instance/instance.hpp"

#include "cmake_facade.hpp"

namespace cmsl
{
    namespace exec
    {
        namespace inst
        {
            project_value::project_value(facade::cmake_facade &cmake_facade,
                                         cmsl::string_view name)
                 : m_cmake_facade{ cmake_facade }
                 , m_name{ name }
            {}

            std::string project_value::name() const
            {
                return m_name.to_string();
            }

            void project_value::add_executable(const std::string &name, const list_value &sources)
            {
                std::vector<std::string> collected_sources;
                collected_sources.reserve(sources.size());

                for(auto i = 0u; i < sources.size(); ++i)
                {
                    const auto& instance = sources.at(i);
                    collected_sources.emplace_back(instance.get_value_cref().get_string_cref());
                }

                m_cmake_facade.add_executable(name, collected_sources);
            }
        }
    }
}
