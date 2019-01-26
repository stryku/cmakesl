#pragma once

#include "sema/sema_type.hpp"

#include <memory>
#include <vector>

namespace cmsl
{
    namespace sema
    {
        class sema_function;
        class sema_type;

        // Todo: change name and move to its own file
        class sema_context_interface
        {
        public:
            virtual ~sema_context_interface() = default;

            virtual void add_function(const sema_function& function) = 0;
            virtual void add_type(std::unique_ptr<sema_type> type) = 0;
            virtual const sema_type* find_type(cmsl::string_view name) const = 0;
            virtual const sema_type* find_type_in_this_scope(cmsl::string_view name) const = 0;
            virtual const sema_function* find_function(cmsl::string_view name) const = 0;
            virtual const sema_function* find_function_in_this_scope(cmsl::string_view name) const = 0;
        };

        class sema_context : public sema_context_interface
        {
        public:
            explicit sema_context(const sema_context_interface* parent = nullptr);

            void add_function(const sema_function& function);
            void add_type(std::unique_ptr<sema_type> type);
            const sema_type* find_type(cmsl::string_view name) const;
            const sema_type* find_type_in_this_scope(cmsl::string_view name) const;
            const sema_function* find_function(cmsl::string_view name) const;
            const sema_function* find_function_in_this_scope(cmsl::string_view name) const;

        private:
            const sema_context_interface* m_parent;
            std::vector<const sema_function*> m_functions;
            std::vector<std::unique_ptr<sema_type>> m_types;
        };
    }
}
