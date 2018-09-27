#pragma once

namespace cmsl
{
    namespace exec
    {
        namespace inst
        {
            class instance;
        }

        namespace infix
        {
            class infix_evaluation_context;
            class infix_expression_visitor;

            class infix_expression
            {
            public:
                virtual ~infix_expression() = default;

                virtual inst::instance* evaluate(infix_evaluation_context& ctx) const = 0;
                virtual void visit(infix_expression_visitor& visitor) const = 0;
            };
        }
    }
}
