set(CMAKESL_TEST_SOURCES
    main.cpp
    mock/add_subdirectory_semantic_handler_mock.hpp
    mock/expression_node_mock.hpp
    mock/identifiers_context_mock.hpp
    mock/sema_context_mock.hpp
    mock/sema_function_mock.hpp
    overload_resolution_test.cpp
    sema_builder_ast_visitor.cpp
    vatiable_initialization_check_test.cpp
)


if(NOT ${CMAKE_VERSION} VERSION_LESS "3.9.0") 
    source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${CMAKESL_TEST_SOURCES})
endif()


set(TEST_NAME sema_test)

add_executable(${TEST_NAME} ${CMAKESL_TEST_SOURCES})
target_link_libraries(${TEST_NAME} gmock_main lexer ast sema errors_observer_mock tests_common)
target_include_directories(${TEST_NAME} PRIVATE 
    ${CMAKESL_SOURCES_DIR}
    ${CMAKESL_TESTS_DIR}
    ${CMAKESL_DIR}
)
add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
