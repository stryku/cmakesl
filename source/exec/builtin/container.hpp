#pragma once

namespace cmsl
{
    namespace exec
    {
        namespace inst
        {
            class instance;
            class instances_holder_interface;
        }

        namespace builtin
        {
            // Class providing basic container methods evaluation
            class container
            {
            public:
                explicit container(inst::instance& class_instance, inst::instances_holder_interface& instances);

                inst::instance* empty();
                inst::instance* size();

            private:
                inst::instance& m_class_instance;
                inst::instances_holder_interface& m_instances;
            };
        }
    }
}
