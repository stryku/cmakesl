#include "cmsl_complete.hpp"
#include "cmsl_parsed_source.hpp"

#include "lexer/lexer.hpp"
#include "ast/parser2.hpp"
#include "ast/ast_node.hpp"
#include "sema/identifiers_context.hpp"
#include "sema/sema_builder.hpp"
#include "sema/builtin_sema_context.hpp"
#include "sema/add_subdirectory_semantic_handler.hpp"
#include "sema/sema_function.hpp"

namespace cmsl::sema::details
{
    class add_subdirectory_handler : public add_subdirectory_semantic_handler
    {
    public:
        const sema_function* handle_add_subdirectory(cmsl::string_view name, const std::vector<std::unique_ptr<expression_node>>& params) override
        {
            return nullptr;
        }
    };
}

cmsl_parsed_source* cmsl_parse_source(const char* source)
{
    auto parsed_source = new cmsl_parsed_source;
    parsed_source->source = source;
    parsed_source->add_subdirectory_handler = std::make_unique<cmsl::sema::details::add_subdirectory_handler>();

    cmsl::source_view source_view{ parsed_source->source };

    cmsl::lexer::lexer lex{ parsed_source->context.errors_observer , source_view };
    const auto tokens = lex.lex();
    cmsl::ast::parser2 parser{ parsed_source->context.errors_observer, source_view, tokens };
    auto ast_tree = parser.translation_unit();

    parsed_source->global_context = std::make_unique<cmsl::sema::builtin_sema_context>(parsed_source->context.type_factory,
                                                                                       parsed_source->context.function_factory,
                                                                                       parsed_source->context.context_factory);

    cmsl::sema::identifiers_context_impl ids_ctx;
    cmsl::sema::sema_builder sema_builder{ *parsed_source->global_context,
                                           parsed_source->context.errors_observer,
                                           ids_ctx,
                                           parsed_source->context.type_factory,
                                           parsed_source->context.function_factory,
                                           parsed_source->context.context_factory,
                                           *parsed_source->add_subdirectory_handler};

    auto sema_tree = sema_builder.build(*ast_tree);

    parsed_source->ast_tree = std::move(ast_tree);
    parsed_source->sema_tree = std::move(sema_tree);

    return parsed_source;
}

void cmsl_destroy_parsed_source(cmsl_parsed_source* parsed_source)
{
    delete parsed_source;
}


cmsl_complete_results* cmsl_complete_at(const cmsl_parsed_source* parsed_source, unsigned absolute_position)
{

}

