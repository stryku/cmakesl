set(ERRORS_SOURCES
    error.hpp
    errors_observer.hpp
    errors_observer.cpp
)

add_library(errors STATIC ${ERRORS_SOURCES})
target_include_directories(errors PRIVATE ${CMAKESL_SOURCES_DIR})
set_target_properties(errors PROPERTIES LINKER_LANGUAGE CXX)

if(NOT ${CMAKE_VERSION} VERSION_LESS "3.9.0") 
    source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX errors FILES ${ERRORS_SOURCES})
endif()
