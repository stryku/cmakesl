#pragma once

// Todo: change name


#include "common/assert.hpp"
#include "exec/instance/instance_value.hpp"

#include <boost/optional.hpp>

#include <unordered_map>
#include <string>
#include <vector>

namespace cmsl
{
    namespace exec
    {
        class value_operations
        {
        public:
            using parameters_t = std::vector<inst::instance_value_t>;
            virtual ~value_operations() = default;

            virtual boost::optional<inst::instance_value_t> execute_operation(std::string name, inst::instance_value_t& val, const parameters_t& params) const = 0;
        };

        template <typename ValueType>
        class value_operations_implementation : public value_operations
        {
        public:
            boost::optional<inst::instance_value_t> execute_operation(std::string name, inst::instance_value_t& val, const parameters_t& params) const override
            {
                auto found = m_operations.find(name);
                if(found == std::cend(m_operations))
                {
                    CMSL_UNREACHABLE("Evaluating unsupported operation");
                    return {};
                }

                const auto& operation = found->second;
                auto& value = boost::get<ValueType>(val);
                return operation(value, params);
            }

        protected:
            using operation_t = std::function<inst::instance_value_t(ValueType& val, const parameters_t&)>;

            void add_operation(std::string name, operation_t op)
            {
                m_operations[name] = op;
            }

            static const ValueType& get_rhs(const parameters_t& params)
            {
                return boost::get<ValueType>(params[0]);
            }

        private:
            std::unordered_map<std::string, operation_t> m_operations;
        };

        class bool_operations : public value_operations_implementation<bool>
        {
        public:
            explicit bool_operations()
            {
                add_operation("&&", [](bool& lhs, const parameters_t& params)
                {
                    return lhs && get_rhs(params);
                });
                add_operation("||", [](bool& lhs, const parameters_t& params)
                {
                    return lhs || get_rhs(params);
                });
                add_operation("to_string", [](bool& lhs, const parameters_t&)
                {
                    return lhs ? "true" : "false";
                });
            }
        };

        class int_operations : public value_operations_implementation<inst::int_t>
        {
        public:
            explicit int_operations()
            {
                add_operation("&&", [](inst::int_t& lhs, const parameters_t& params)
                {
                    return lhs && get_rhs(params);
                });
                add_operation("||", [](inst::int_t& lhs, const parameters_t& params)
                {
                    return lhs || get_rhs(params);
                });
                add_operation("+", [](inst::int_t& lhs, const parameters_t& params)
                {
                    return lhs + get_rhs(params);
                });
                add_operation("-", [](inst::int_t& lhs, const parameters_t& params)
                {
                    return lhs - get_rhs(params);
                });
                add_operation("to_string", [](inst::int_t& lhs, const parameters_t&)
                {
                    return std::to_string(lhs);
                });
            }
        };
    }
}
