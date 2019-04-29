#include "type_names_collector.hpp"

#include "sema/sema_nodes.hpp"
#include "sema/sema_context.hpp"

namespace cmsl::tools
{
    std::unordered_set<std::string> type_names_collector::collect(const sema::sema_context& builtin_context, const sema::sema_node &start_node) const
    {
        std::unordered_set<std::string> type_names;

        auto current_node = &start_node;
        auto last_node = current_node;

        // Currently, namespaces are not supported, so classes can be defined only in a global scope.
        // To find all classes, just find the root, which is the translation unit node, iterate over its node,
        // and if a given node is class, collect its name.

        while(current_node->parent())
        {
            last_node = current_node;
            current_node = current_node->parent();
        }

        const auto translation_unit = dynamic_cast<const sema::translation_unit_node*>(current_node);

        for(const auto& top_level_node : translation_unit->nodes())
        {
            if(top_level_node.get() == last_node)
            {
                break;
            }

            if(const auto class_ = dynamic_cast<const sema::class_node*>(top_level_node.get()))
            {
                type_names.emplace(class_->name().str());
            }
        }

        const auto builtin_types = builtin_context.types();
        for(const auto& ty : builtin_types)
        {
            if(!ty.get().is_reference())
            {
                type_names.emplace(ty.get().name().to_string());
            }
        }

        return type_names;
    }
}
