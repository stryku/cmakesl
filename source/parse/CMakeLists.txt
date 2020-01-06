set(PARSE_SOURCES
    parse_errors_observer.hpp
    parser_utils.cpp
    parser_utils.hpp
)

add_library(parse "${PARSE_SOURCES}")

target_include_directories(parse
    PRIVATE
        ${CMAKESL_SOURCES_DIR}
)

target_link_libraries(parse
    PUBLIC
        lexer
)

target_compile_options(parse
    PRIVATE
        ${CMAKESL_ADDITIONAL_COMPILER_FLAGS}
)

