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

        // Todo: change name
        class sema_context_interface
        {
        public:
            virtual ~sema_context_interface() = default;

            virtual void add_function(std::unique_ptr<sema_function> function) = 0;
            virtual void add_type(std::unique_ptr<sema_type> type) = 0;
            virtual const sema_type* find_type(cmsl::string_view name) const = 0;
            virtual const sema_type* find_type_in_this_scope(cmsl::string_view name) const = 0;
            virtual const sema_function* find_function(cmsl::string_view name) const = 0;
            virtual const sema_function* find_function_in_this_scope(cmsl::string_view name) const = 0;
        };

        class sema_context : public sema_context_interface
        {
        public:
            explicit sema_context(const sema_context* parent = nullptr)
                : m_parent{ parent }
            {}

            void add_function(std::unique_ptr<sema_function> function)
            {
                m_functions.emplace_back(std::move(function));
            }

            void add_type(std::unique_ptr<sema_type> type)
            {
                m_types.emplace_back(std::move(type));
            }

            const sema_type* find_type(cmsl::string_view name) const
            {
                if(auto found = find_type_in_this_scope(name))
                {
                    return found;
                }

                return m_parent ? m_parent->find_type(name) : nullptr;
            }

            const sema_type* find_type_in_this_scope(cmsl::string_view name) const
            {
                const auto pred = [name](const auto& type)
                {
                    return name == type->name();
                };

                const auto found = std::find_if(std::cbegin(m_types), std::cend(m_types), pred);

                return found != std::cend(m_types) ? found->get() : nullptr;
            }

            const sema_function* find_function(cmsl::string_view name) const
            {
                if(auto found = find_function_in_this_scope(name))
                {
                    return found;
                }

                return m_parent ? m_parent->find_function(name) : nullptr;
            }

            const sema_function* find_function_in_this_scope(cmsl::string_view name) const
            {
                const auto pred = [name](const auto& function)
                {
                    return name == function->name();
                };

                const auto found = std::find_if(std::cbegin(m_functions), std::cend(m_functions), pred);
                return found != std::cend(m_functions) ? found->get() : nullptr;
            }

        private:
            const sema_context* m_parent;
            std::vector<std::unique_ptr<sema_function>> m_functions;
            std::vector<std::unique_ptr<sema_type>> m_types;
        };
    }
}
