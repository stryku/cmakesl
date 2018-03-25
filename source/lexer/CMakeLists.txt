set(LEXER_SOURCES
    lexer.hpp
    lexer.cpp
    
    token/token.hpp
    token/token_type.hpp
)

add_library(lexer ${LEXER_SOURCES})
target_include_directories(lexer PRIVATE ${CMAKESL_SOURCES_DIR})

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX lexer FILES ${LEXER_SOURCES})
