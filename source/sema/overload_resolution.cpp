#include "sema/overload_resolution.hpp"
#include "sema/sema_nodes.hpp"
#include "sema/sema_function.hpp"
#include "errors/errors_observer.hpp"
#include "errors/error.hpp"
#include "overload_resolution.hpp"


namespace cmsl
{
    namespace sema
    {
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
                if(call_parameters[i]->type() != signature.params[i].ty)
                {
                    return false;
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
                m_errs.nofify_error({m_call_token.src_range(), "Function call parameters does not match"});
            }
            else
            {
                m_errs.nofify_error({m_call_token.src_range(), "No matching function fot call"});
            }
        }
    }
}
