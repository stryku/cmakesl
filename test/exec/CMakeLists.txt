set(TESTS_NAME exec_test)

set(SOURCES 
    main.cpp
        instance_converter_test.cpp
    test_context_provider.hpp
    high_level_exec_test.cpp
    builtin_type_default_constructor.cpp
    fundamental_value_expression_test.cpp
    binary_operator_expression_test.cpp
    eval/ExpressionEvaluationTest.hpp
    id_expression_test.cpp
    user_class_member_access_expression_test.cpp
    mock/instance_mock.hpp
    mock/infix_expression_mock.hpp
    function_call_expression_test.cpp
    mock/function_caller_mock.hpp
    user_member_function_call_expression_test.cpp
    infix_expression_builder_test.cpp
    mock/infix_expression_visitor_mock.hpp
)


if(NOT ${CMAKE_VERSION} VERSION_LESS "3.9.0") 
    source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} )
endif()


add_executable(${TESTS_NAME} ${SOURCES} )
target_link_libraries(${TESTS_NAME} gmock_main exec lexer errors ast tests_common)
target_include_directories(${TESTS_NAME} PRIVATE ${CMAKESL_SOURCES_DIR} ${CMAKESL_DIR})
add_test(NAME ${TESTS_NAME} COMMAND ${TESTS_NAME}) 
