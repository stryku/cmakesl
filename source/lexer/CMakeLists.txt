set(LEXER_SOURCES
    lexer.cpp
    lexer.hpp
    source_location_manipulator.cpp
    source_location_manipulator.hpp
    token.cpp
    token.hpp
    token_type.cpp
    token_type.hpp
)

add_library(lexer ${LEXER_SOURCES})
target_include_directories(lexer PRIVATE ${CMAKESL_SOURCES_DIR})
target_link_libraries(lexer common)

if(NOT ${CMAKE_VERSION} VERSION_LESS "3.9.0") 
    source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX lexer FILES ${LEXER_SOURCES})
endif()

