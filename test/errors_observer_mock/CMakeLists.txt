set(ERRORS_OBSERVER_MOCK_SOURCES
    errors_observer_mock.cpp
)

set(PROJECT_NAME errors_observer_mock)

add_library(${PROJECT_NAME} STATIC ${ERRORS_OBSERVER_MOCK_SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE 
    ${CMAKESL_SOURCES_DIR}
    ${CMAKESL_TESTS_DIR}
    "${gtest_SOURCE_DIR}/include" 
    "${gmock_SOURCE_DIR}/include"
)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX ${PROJECT_NAME} FILES ${ERRORS_OBSERVER_MOCK_SOURCES})
