set(CMSL_TOOLS_SOURCES
    cmsl_complete.cpp
    cmsl_complete.hpp
    cmsl_index.cpp
    cmsl_index.hpp
    cmsl_parse_source.cpp
    cmsl_parse_source.hpp
    cmsl_parsed_source.hpp
    completer.cpp
    completer.hpp
    completion_context_finder.cpp
    completion_context_finder.hpp
    completion_contextes_visitor.cpp
    completion_contextes_visitor.hpp
    identifier_names_collector.cpp
    identifier_names_collector.hpp
    type_names_collector.cpp
    type_names_collector.hpp
)

add_library(cmsl_tools ${CMSL_TOOLS_SOURCES})

target_include_directories(cmsl_tools
    PRIVATE
        ${CMAKESL_SOURCES_DIR}
        ${CMAKESL_FACADE_DIR}
)

target_link_libraries(cmsl_tools 
    PRIVATE
    sema
    errors
)
