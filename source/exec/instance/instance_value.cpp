#include "exec/instance/instance_value.hpp"

namespace cmsl
{
    namespace exec
    {
        namespace inst
        {
            instance_value_type get_inst_val_type(const instance_value_t& i)
            {
                return static_cast<instance_value_type>(i.which());
            }
        }
    }
}
