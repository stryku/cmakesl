#include "ast/type_conversion_check.hpp"
#include "ast/type_kind.hpp"


#include <gmock/gmock.h>

namespace cmsl
{
    namespace ast
    {
        namespace test
        {
            namespace type_conversion_checker
            {
                TEST(TypeConversionCheck_CanConvert, FundamentalArithmetical_ConversionPossible)
                {
                    const auto kinds = {type_kind::k_bool,
                                        type_kind::k_int,
                                        type_kind::k_double};

                    type_conversion_check tcc;

                    for (const auto from : kinds)
                    {
                        for (const auto to : kinds)
                        {
                            const auto result = tcc.can_convert(from, to);
                            ASSERT_THAT(result, true);
                        }
                    }
                }

                TEST(TypeConversionCheck_CanConvert, FundamentalToFromUser_ConversionNotPossible)
                {
                    const auto kinds = {type_kind::k_bool,
                                        type_kind::k_int,
                                        type_kind::k_double};

                    type_conversion_check tcc;

                    for (const auto kind : kinds)
                    {
                        {
                            const auto result = tcc.can_convert(kind, type_kind::k_user);
                            ASSERT_THAT(result, false);
                        }
                        {
                            const auto result = tcc.can_convert(type_kind::k_user, kind);
                            ASSERT_THAT(result, false);
                        }
                    }
                }
            }
        }
    }
}
