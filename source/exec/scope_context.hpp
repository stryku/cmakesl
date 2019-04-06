#pragma once

#include "common/string.hpp"
#include "exec/instance/instance.hpp"

#include <unordered_map>

namespace cmsl::exec
{
        class scope_context
        {
        private:
            using instance_t = inst::instance;

        public:
            // Used when creating initial function scope or scope in already created scope
            explicit scope_context(scope_context* parent);

            // Used when entering a method which is member of class_instance
            explicit scope_context(instance_t* class_instance);

            void add_variable(cmsl::string_view name, std::unique_ptr<instance_t> inst);
            instance_t* get_variable(cmsl::string_view name);
            const instance_t* get_variable(cmsl::string_view name) const;

            bool variable_exists(cmsl::string_view name) const;

        private:
            string_view_map<std::unique_ptr<instance_t>> m_variables;
            scope_context* m_parent;
            instance_t* m_class_instance;
        };
}
