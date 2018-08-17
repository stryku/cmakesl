set(AST_TEST_SOURCES 
    main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ast_context.cpp 
)


if(NOT ${CMAKE_VERSION} VERSION_LESS "3.9.0") 
    source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${AST_TEST_SOURCES})
endif()


set(TEST_NAME ast_test)

add_executable(${TEST_NAME} ${AST_TEST_SOURCES})
target_link_libraries(${TEST_NAME} gmock_main lexer errors ast)
target_include_directories(${TEST_NAME} PRIVATE ${CMAKESL_SOURCES_DIR})
add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
