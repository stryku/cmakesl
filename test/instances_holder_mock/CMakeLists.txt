set(INSTANCES_HOLDER_MICK_SOURCES
        instances_holder_mock.cpp
)

set(PROJECT_NAME instances_holder_mock)

add_library(${PROJECT_NAME} STATIC ${INSTANCES_HOLDER_MICK_SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE 
    ${CMAKESL_SOURCES_DIR}
    ${CMAKESL_TESTS_DIR}
    "${gtest_SOURCE_DIR}/include" 
    "${gmock_SOURCE_DIR}/include"
)

if(NOT ${CMAKE_VERSION} VERSION_LESS "3.9.0") 
    source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX ${PROJECT_NAME} FILES ${INSTANCES_HOLDER_MICK_SOURCES})
endif()
