set(CMAKESL_TEST_SOURCES 
    main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/lexer_test.cpp 
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${CMAKESL_TEST_SOURCES})

add_executable(cmakesl_lexer_test ${CMAKESL_TEST_SOURCES})
target_link_libraries(cmakesl_lexer_test gmock_main common lexer errors)
target_include_directories(cmakesl_lexer_test PRIVATE ${CMAKESL_SOURCES_DIR})
add_test(NAME cmakesl_lexer_test COMMAND cmakesl_lexer_test)
