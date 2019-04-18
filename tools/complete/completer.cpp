#include "completer.hpp"
#include "completion_context_finder.hpp"
#include "cmsl_parsed_source.hpp"
#include "completion_contextes_visitor.hpp"
#include "cmsl_complete.hpp"

namespace cmsl::tools
{
    completer::completer(const cmsl_parsed_source &parsed_source, unsigned absolute_position)
        : m_parsed_source{ parsed_source }
        , m_absolute_position{ absolute_position }
    {}

    cmsl_complete_results *completer::complete()
    {
        auto finder = completion_context_finder{ m_absolute_position };
        m_parsed_source.sema_tree->visit(finder);
        const auto found_context = finder.result();

        if(std::holds_alternative<could_not_find_context>(found_context))
        {
            return nullptr;
        }

        auto results = new cmsl_complete_results;
        auto visitor = completion_contextes_visitor{m_parsed_source, *results};
        std::visit(visitor, found_context);
        visitor.finalize();

        return results;
    }
}
