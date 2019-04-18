#include "cmsl_index.hpp"

#include "sema/sema_node_visitor.hpp"
#include "sema/sema_nodes.hpp"

#include <vector>
#include <ast/variable_declaration_node.hpp>
#include <cstring>

namespace cmsl
{
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

    virtual void visit(const sema::variable_declaration_node& node) override
    {
        const auto& ast_node = node.ast_node();
        const auto variable_decl = dynamic_cast<const ast::variable_declaration_node*>(&ast_node);

        cmsl_index_entry type_entry;
        add_entry_data(type_entry, variable_decl->get_type_representation(), node.type());
        type_entry.start_pos = variable_decl->get_type_representation().tokens().front().src_range().begin.absolute;
        type_entry.end_pos = variable_decl->get_type_representation().tokens().back().src_range().end.absolute;
        m_intermediate_entries.emplace_back(type_entry);

        // todo add variable name to context

        if(node.initialization())
        {
            node.initialization()->visit(*this);
        }
    }

private:
    void add_entry_data(cmsl_index_entry& entry, const ast::type_representation& type_representation, const sema::sema_type& type)
    {
        entry.position = type.name().tokens().front().src_range().begin.absolute;
        add_entry_path(entry, type.name().tokens().front().source().path());
    }

    void add_entry_path(cmsl_index_entry& entry, cmsl::string_view path)
    {
        entry.source_path = new char[path.size() + 1u];
        std::strcpy(entry.source_path, path.data());
    }

private:
    std::vector<cmsl_index_entry> m_intermediate_entries;
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
