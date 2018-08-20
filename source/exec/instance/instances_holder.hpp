#pragma once

#include <memory>
#include <vector>

namespace cmsl
{
    namespace exec
    {
        class executor;

        namespace inst
        {
            class instance;
            class instance_factory;

            // class used to handle lifetime of 'rvalue' instances created during full expression execution
            class instances_holder
            {
            public:
                explicit instances_holder(executor& e);

                inst::instance* create(int value);

            private:
                inst::instance_factory get_factory();

            private:
                executor& m_exec;
                std::vector<std::unique_ptr<inst::instance>> m_instances;
            };
        }
    }
}
