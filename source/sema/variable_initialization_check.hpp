#pragma once

namespace cmsl::sema
{
    class sema_type;
    class expression_node;

    class variable_initialization_checker
    {
    public:
        enum class check_result
        {
            can_init,
            different_types,
            reference_init_from_temporary_value
        };

        check_result can_initialize(const sema_type& variable_type, const expression_node& initialization_expression) const;

    private:
        check_result check_initialization_of_reference_type(const sema_type& variable_type, const expression_node& initialization_expression) const;
        check_result check_initialization(const sema_type& variable_type, const expression_node& initialization_expression) const;
    };
}
