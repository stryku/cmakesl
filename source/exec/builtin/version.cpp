#include "exec/builtin/version.hpp"

#include "exec/instance/instances_holder.hpp"
#include "exec/instance/instance.hpp"

namespace cmsl
{
    namespace exec
    {
        namespace builtin
        {
            version_ctor::version_ctor(inst::instances_holder_interface &instances,
                                       const ast::type& version_type,
                                       const inst::instance &major,
                                       const inst::instance &minor,
                                       const inst::instance &patch,
                                       const inst::instance &tweak)
                : evaluatable_function(instances)
                ,m_version_type{ version_type }
                ,m_major{ major }
                ,m_minor{ minor }
                ,m_patch{ patch }
                ,m_tweak{ tweak }
            {}

            inst::instance *version_ctor::evaluate()
            {
                inst::instance_members_t members;
                members.emplace("major", m_major.copy());
                members.emplace("minor", m_minor.copy());
                members.emplace("patch", m_patch.copy());
                members.emplace("tweak", m_tweak.copy());

                return m_instances.create(m_version_type, std::move(members));
            }
        }
    }
}
