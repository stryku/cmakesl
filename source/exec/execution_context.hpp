#pragma once

#include "exec/scope_context.hpp"

#include <stack>

namespace cmsl
{
    namespace exec
    {
        class execution_context
        {
        private:
            using instance_t = inst::instance;

        public:
            void add_variable(cmsl::string_view name, std::unique_ptr<instance_t> inst);
            instance_t* get_variable(cmsl::string_view name);

            bool variable_exists(cmsl::string_view name) const;

            void enter_scope();
            void enter_member_function_scope(instance_t* class_instance);
            void leave_scope();

        private:
            scope_context* current_scope();
            const scope_context* current_scope() const;

        private:
            std::stack<scope_context> m_scopes;
        };
    }
}
