#include "identifier_names_collector.hpp"

#include "sema/sema_nodes.hpp"
#include "sema/sema_context.hpp"

namespace cmsl::tools
{
    class identifiers_collector_visitor : public sema::empty_sema_node_visitor
    {
    public:
        explicit identifiers_collector_visitor(const sema::sema_node &last_node, std::unordered_set<std::string>& result)
            : m_last_node{ last_node }
            , m_result{ result }
        {}

        void visit(const sema::block_node& node ) override
        {
            for(const auto& sub_node : node.nodes())
            {
                if(sub_node.get() == &m_last_node)
                {
                    return;
                }

                if(const auto variable_decl = dynamic_cast<const sema::variable_declaration_node*>(sub_node.get()))
                {
                    m_result.emplace(variable_decl->name().str());
                }
            }
        }

        void visit(const sema::function_node& node) override
        {
            // Identifiers from the function body are already collected, because we go up in the tree. We already were in this function body. Let's collect function and params names.

            const auto& signature = node.signature();
            m_result.emplace(signature.name.str());

            for(const auto& param_decl : signature.params)
            {
                m_result.emplace(param_decl.name.str());
            }
        }

        void visit(const sema::class_node& node) override
        {
            // Todo: handle case when we don't start from the function body. E.g. we start in the class scope.

            for(const auto& member_decl : node.members())
            {
                m_result.emplace(member_decl->name().str());
            }

            for(const auto& function : node.functions())
            {
                m_result.emplace(function->signature().name.str());
            }
        }

        void visit(const sema::translation_unit_node& node) override
        {
            for(const auto& sub_node : node.nodes())
            {
                if(sub_node.get() == &m_last_node)
                {
                    return;
                }

                if(const auto variable_decl = dynamic_cast<const sema::variable_declaration_node*>(sub_node.get()))
                {
                    m_result.emplace(variable_decl->name().str());
                }
                else if(const auto function_decl = dynamic_cast<const sema::function_node*>(sub_node.get()))
                {
                    m_result.emplace(function_decl->signature().name.str());
                }
            }
        }

    private:
        const sema::sema_node &m_last_node;
        std::unordered_set<std::string>& m_result;
    };

    std::unordered_set<std::string> identifier_names_collector::collect(const sema::sema_node &start_node) const
    {
        std::unordered_set<std::string> result;

        auto current_node = &start_node;
        auto last_node = current_node;

        while(current_node != nullptr)
        {
            auto collector = identifiers_collector_visitor{ *last_node, result };

            current_node->visit(collector);

            last_node = current_node;
            current_node = current_node->parent();
        }

        return result;
    }
}
