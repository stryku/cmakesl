#include "exec/stmt/builtin_call_size_statement.hpp"

#include "exec/exec.hpp"
#include "common/type_traits.hpp"

namespace cmsl
{
    namespace exec
    {
        namespace stmt
        {
            namespace details
            {
                struct size_getter_visitor : public boost::static_visitor<inst::instance_value_t>
                {
                    template <typename ValueType>
                    auto operator()(ValueType&& val) const -> decltype(static_cast<int>(val.size()))
                    {
                        return static_cast<int>(val.size());
                    }

                    template <typename T,
                              typename = enable_if_not_container<T>>
                    inst::instance_value_t operator()(T&&) const
                    {
                        // Should never be called
                        return 0;
                    }
                };
            }

            void builtin_call_size_statement::execute(executor &e)
            {
                auto class_this = e.get_exec_ctx().get_this();
                const auto this_val = class_this->get_value();
                auto result = boost::apply_visitor(details::size_getter_visitor{}, this_val);
                e.return_from_function(result);
            }
        }
    }
}
