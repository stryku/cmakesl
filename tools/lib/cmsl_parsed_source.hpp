#pragma once

#include "sema/sema_node.hpp"
#include "sema/sema_tree_building_context.hpp"

#include <memory>
#include <string>


namespace cmsl::sema
{
    class add_subdirectory_handler;
}

struct cmsl_parsed_source
{
    std::string source;
    cmsl::sema::sema_tree_building_context context;
    std::unique_ptr<cmsl::sema::add_subdirectory_handler> add_subdirectory_handler;
    std::unique_ptr<cmsl::ast::ast_node> ast_tree;
    std::unique_ptr<cmsl::sema::sema_node> sema_tree;
    std::unique_ptr<cmsl::sema::sema_context> builtin_context;
};
