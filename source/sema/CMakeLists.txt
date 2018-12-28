set(SEMA_SOURCES
        sema_builder.cpp sema_builder.hpp)

add_library(sema ${SEMA_SOURCES})
target_include_directories(sema PRIVATE ${CMAKESL_SOURCES_DIR})

if(NOT ${CMAKE_VERSION} VERSION_LESS "3.9.0") 
    source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX sema FILES ${SEMA_SOURCES})
endif()
