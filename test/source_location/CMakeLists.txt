set(SOURCE_LOCATION_TEST_SOURCES 
    main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source_location_test.cpp 
)

set(TESTS_NAME source_location_test)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCE_LOCATION_TEST_SOURCES})

add_executable(${TESTS_NAME} ${SOURCE_LOCATION_TEST_SOURCES})
target_link_libraries(${TESTS_NAME} gmock_main)
target_include_directories(${TESTS_NAME} PRIVATE ${CMAKESL_SOURCES_DIR})
add_test(NAME ${TESTS_NAME} COMMAND ${TESTS_NAME})
