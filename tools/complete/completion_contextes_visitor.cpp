#include "completion_contextes_visitor.hpp"
#include "completer.hpp"

#include "sema/sema_nodes.hpp"
#include "cmsl_complete.hpp"

#include <cstring>

namespace cmsl::tools
{
    completion_contextes_visitor::completion_contextes_visitor(const cmsl_parsed_source &parsed_source, cmsl_complete_results &results)
        : m_parsed_source{ parsed_source }
        , m_results{ results }
    {}

    void completion_contextes_visitor::operator()(const standalone_expression_context &ctx)
    {
        if(const auto block = dynamic_cast<const sema::block_node*>(&ctx.node.get()))
        {
            add_standalone_expression_keywords();
        }
    }

    void completion_contextes_visitor::add_standalone_expression_keywords()
    {
        const auto keywords = {
                "if",
                "for",
                "while"
        };

        for(const auto& keyword : keywords)
        {
            m_intermediate_results.emplace_back(keyword);
        }
    }

    void completion_contextes_visitor::finalize()
    {
        if(m_intermediate_results.empty())
        {
            return;
        }

        m_results.num_results = static_cast<unsigned>(m_intermediate_results.size());
        m_results.results = new char*[m_intermediate_results.size()];

        for(auto i = 0u; i < m_intermediate_results.size(); ++i)
        {
            const auto& str = m_intermediate_results[i];
            m_results.results[i] = new char[str.size() + 1];
            std::strcpy(m_results.results[i], str.c_str());
        }
    }
}
