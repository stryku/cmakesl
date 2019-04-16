#include "type_names_collector.hpp"

#include "sema/sema_nodes.hpp"
#include "sema/sema_context.hpp"

namespace cmsl::tools
{
    std::vector<std::string> type_names_collector::collect(const sema::sema_node &start_node) const
    {
        std::vector<std::string> type_names;

        auto current_node = &start_node;
        auto last_node = current_node;
        while(current_node->parent())
        {
            last_node = current_node;
            current_node = current_node->parent();
        }

        const auto translation_unit = dynamic_cast<const sema::translation_unit_node*>(current_node);

        const auto builtin_types = translation_unit->context().types();

        for(const auto& ty : builtin_types)
        {
            if(!ty.get().is_reference())
            {
                type_names.emplace_back(ty.get().name().to_string());
            }
        }

        for(const auto& top_level_node : translation_unit->nodes())
        {
            if(top_level_node.get() == last_node)
            {
                break;
            }

            if(const auto class_ = dynamic_cast<const sema::class_node*>(top_level_node.get()))
            {
                type_names.emplace_back(class_->name().str());
            }
        }

        return type_names;
    }
}
