set(CMSL_EXECUTABLE_SOURCES
    main.cpp
)

add_executable(cmakesl ${CMSL_EXECUTABLE_SOURCES})
target_include_directories(cmakesl PRIVATE ${CMAKESL_SOURCES_DIR} ${CMAKESL_FACADE_DIR})
target_link_libraries(cmakesl PRIVATE exec sema errors )

if(NOT ${CMAKE_VERSION} VERSION_LESS "3.9.0") 
    source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX cmakesl FILES ${CMSL_EXECUTABLE_SOURCES})
endif()

