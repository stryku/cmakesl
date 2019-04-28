set(LEXER_SOURCES
        token.hpp
        token.cpp
        token_type.hpp
    
    lexer.hpp
    lexer.cpp
    
    source_location_manipulator.hpp
    source_location_manipulator.cpp
        token_type.cpp)

add_library(lexer ${LEXER_SOURCES})
target_include_directories(lexer PRIVATE ${CMAKESL_SOURCES_DIR})
target_link_libraries(lexer common)

if(NOT ${CMAKE_VERSION} VERSION_LESS "3.9.0") 
    source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX lexer FILES ${LEXER_SOURCES})
endif()

