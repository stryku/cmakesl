set(LEXER_SOURCES
    lexer.hpp
    lexer.cpp
    token.hpp
)

add_library(lexer ${LEXER_SOURCES})
target_include_directories(lexer PRIVATE ${CMAKESL_SOURCES_DIR})

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${LEXER_SOURCES})
