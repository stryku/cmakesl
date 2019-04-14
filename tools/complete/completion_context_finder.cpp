#include "completion_context_finder.hpp"

#include "sema/sema_nodes.hpp"

namespace cmsl::tools
{
    completion_context_finder::completion_context_finder(unsigned absolute_position)
        : m_pos{ 1u, 1u, absolute_position }
    {}

    void completion_context_finder::visit(const sema::translation_unit_node &node)
    {
        for(const auto& sub_node : node.nodes())
        {
            if(is_inside(*sub_node))
            {
                sub_node->visit(*this);
            }
        }
    }

    bool completion_context_finder::is_inside(const sema::sema_node &node) const
    {
        return node.begin_location() <= m_pos && m_pos <= node.end_location();
    }

    bool completion_context_finder::is_before(const sema::sema_node &node) const
    {
        return m_pos < node.begin_location();
    }

    bool completion_context_finder::is_pos_before_node_begin(const sema::sema_node &node) const
    {
        return m_pos < node.end_location();
    }

    void completion_context_finder::visit(const sema::function_node &node)
    {
        node.body().visit(*this);
    }

    void completion_context_finder::visit(const sema::block_node &node)
    {
        unsigned place{ 0u };

        for(const auto& sub_node : node.nodes())
        {
            if(!is_pos_before_node_begin(*sub_node))
            {
                // Now, we know that sub_node begins after pos that we want to find.
                // That means that the pos is between two nodes.
                // So we know that here can be a standalone expression only, thus we return such context.
                m_result = standalone_expression_context{ node, place };
                return;
            }

            if(is_inside(*sub_node))
            {
                sub_node->visit(*this);
                return;
            }

            ++place;
        }
    }

    completion_context_t completion_context_finder::result() const
    {
        return m_result;
    }
}
