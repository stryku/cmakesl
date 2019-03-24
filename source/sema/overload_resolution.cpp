#include "sema/overload_resolution.hpp"
#include "sema/sema_nodes.hpp"
#include "sema/sema_function.hpp"
#include "sema/variable_initialization_checker.hpp"
#include "errors/errors_observer.hpp"
#include "errors/error.hpp"


namespace cmsl
{
    namespace sema
    {
        // Todo: While choosing a function, diagnostic for all possible funciton should be collected.
        // Then, if there is an unambiguous function, return it. If not, provide a meaningful error.

        overload_resolution::overload_resolution(errors::errors_observer &errs, lexer::token::token call_token)
            : m_errs{ errs }
            , m_call_token{ call_token }
        {}

        const sema_function*
        overload_resolution::choose(const function_lookup_result_t &functions,
                                    const std::vector<std::unique_ptr<expression_node>> &call_parameters) const
        {
            for(const auto& scoped_functions : functions)
            {
                // Todo: is it even possible to have empty vector here?
                if(scoped_functions.empty())
                {
                    continue;
                }

                const auto chosen_function = choose_from_scope(scoped_functions, call_parameters);
                if(chosen_function == nullptr)
                {
                    raise_wrong_call_error(scoped_functions, call_parameters);
                }

                return chosen_function;
            }

            m_errs.nofify_error({m_call_token.src_range(), '\'' + m_call_token.str().to_string() + "\' function not found"});

            return nullptr;
        }

        const sema_function*
        overload_resolution::choose(const single_scope_function_lookup_result_t &functions,
                                    const std::vector<std::unique_ptr<expression_node>> &call_parameters) const
        {
            // Todo: is it even possible to have empty vector here?
            if(functions.empty())
            {
                m_errs.nofify_error({m_call_token.src_range(), '\'' + m_call_token.str().to_string() + "\' function not found"});
                return nullptr;
            }

            const auto chosen_function = choose_from_scope(functions, call_parameters);
            if(chosen_function == nullptr)
            {
                raise_wrong_call_error(functions, call_parameters);
            }

            return chosen_function;
        }



        const sema_function *
        overload_resolution::choose_from_scope(const single_scope_function_lookup_result_t &functions,
                                               const std::vector<std::unique_ptr<expression_node>> &call_parameters) const
        {
            for(const auto function : functions)
            {
                if(params_match(*function, call_parameters))
                {
                    return function;
                }
            }

            return nullptr;
        }

        bool
        overload_resolution::params_match(const sema_function &function, const std::vector<std::unique_ptr<expression_node>> &call_parameters) const
        {
            const auto& signature = function.signature();

            if(signature.params.size() != call_parameters.size())
            {
                return false;
            }

            for(auto i = 0u; i < call_parameters.size(); ++i)
            {
                const auto& declared_param_type = signature.params[i].ty;
                const auto& param_expression_type = call_parameters[i]->type();

                variable_initialization_checker checker;
                const auto init_check_result = checker.check(declared_param_type, *call_parameters[i]);

                using check_result_t = variable_initialization_checker::check_result;
                switch(init_check_result)
                {
                    case check_result_t::can_init:
                    {
                        continue;
                    }
                    case check_result_t::different_types:
                    case check_result_t::reference_init_from_temporary_value:
                    {
                        return false;
                    }
                }
            }

            return true;
        }

        void
        overload_resolution::raise_wrong_call_error(const single_scope_function_lookup_result_t &functions, const std::vector<std::unique_ptr<expression_node>> &call_parameters) const
        {
            // Todo: implement function::to_string to have a nicely printed error
            // Todo: improve error explanation
            if(functions.size() == 1u)
            {
                m_errs.nofify_error({m_call_token.src_range(), "Function call parameters does not match"}); // Todo: more meaningful error
            }
            else
            {
                m_errs.nofify_error({m_call_token.src_range(), "No matching function for call"});
            }
        }

        const sema_function *
        overload_resolution::choose(const single_scope_function_lookup_result_t &functions, const expression_node &call_parameter) const
        {
            for(const auto function : functions)
            {
                const auto& params = function->signature().params;
                if(params.size() == 1u && params[0].ty == call_parameter.type())
                {
                    return function;
                }
            }

            raise_wrong_call_error(functions, call_parameter);

            return nullptr;
        }

        void
        overload_resolution::raise_wrong_call_error(const single_scope_function_lookup_result_t &functions, const expression_node &call_parameter) const
        {
            // Todo: implement function::to_string to have a nicely printed error
            // Todo: improve error explanation
            if(functions.size() == 1u)
            {
                m_errs.nofify_error({m_call_token.src_range(), "Function call parameters does not match"}); // Todo: more meaningful error
            }
            else
            {
                m_errs.nofify_error({m_call_token.src_range(), "No matching function for call"});
            }
        }
    }
}
