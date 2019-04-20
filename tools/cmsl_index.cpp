#include "cmsl_index.hpp"

#include "sema/identifiers_context.hpp"
#include "sema/sema_node_visitor.hpp"
#include "sema/sema_nodes.hpp"

#include <vector>
#include <ast/variable_declaration_node.hpp>
#include <cstring>

namespace cmsl
{
    cmsl_index_entry make_entry(const lexer::token::token& token, cmsl_index_entry_type type, string_view path, unsigned position)
    {
        cmsl_index_entry entry{};

        entry.begin_pos = token.src_range().begin.absolute;
        entry.end_pos = token.src_range().end.absolute;

        entry.type = type;

        entry.source_path = new char[path.size() + 1u];
        std::strcpy(entry.source_path, path.data());

        entry.position = position;

        return entry;
    }


class indexer : public sema::sema_node_visitor
{
public:
    void visit(const sema::translation_unit_node& node) override
    {
        for(const auto& sub_node : node.nodes())
        {
            sub_node->visit(*this);
        }
    }

    void visit(const sema::variable_declaration_node& node) override
    {
        const auto& ast_node = node.ast_node();
        const auto variable_decl = dynamic_cast<const ast::variable_declaration_node*>(&ast_node);

        const auto type_entry = make_entry(variable_decl->get_type_representation().primary_name(),
                                           cmsl_index_entry_type::type,
                                           node.type().name().primary_name().source().path(),
                                           node.type().name().primary_name().src_range().begin.absolute);

        m_intermediate_entries.emplace_back(type_entry);

        m_identifiers_context.register_identifier(variable_decl->get_name(), &node.type());

        if(node.initialization())
        {
            node.initialization()->visit(*this);
        }
    }

private:
    std::vector<cmsl_index_entry> m_intermediate_entries;
    sema::identifiers_context_impl m_identifiers_context;
};
}


struct cmsl_index_entries* cmsl_index(const struct cmsl_parsed_source* parsed_source, unsigned absolute_position)
{

}

void cmsl_index(struct cmsl_index_entries* index_entries)
{
    for(auto i = 0u; i < index_entries->num_entries; ++i )
    {
        delete [] index_entries->entries[i].source_path;
    }

    delete [] index_entries->entries;
    delete index_entries;
}
