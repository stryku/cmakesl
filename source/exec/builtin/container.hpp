#pragma once

namespace cmsl
{
    namespace exec
    {
        namespace inst
        {
            class instance;
            class instances_holder;
        }

        namespace builtin
        {
            // Class providing basic container methods evaluation
            class container
            {
            public:
                explicit container(inst::instance& class_instance, inst::instances_holder& instances);

                inst::instance* empty();
                inst::instance* size();

            private:
                inst::instance& m_class_instance;
                inst::instances_holder& m_instances;
            };
        }
    }
}
