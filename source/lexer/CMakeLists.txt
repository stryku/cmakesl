set(LEXER_SOURCES
    token/token.hpp
    token/token.cpp
    token/token_type.hpp
    
    lexer.hpp
    lexer.cpp
    
    source_location_manipulator.hpp
    source_location_manipulator.cpp
)

add_library(lexer ${LEXER_SOURCES})
target_include_directories(lexer PRIVATE ${CMAKESL_SOURCES_DIR})

if(NOT ${CMAKE_VERSION} VERSION_LESS "3.9.0") 
    source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX lexer FILES ${LEXER_SOURCES})
endif()

