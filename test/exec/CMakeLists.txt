set(TESTS_NAME exec_test)

set(SOURCES
    bool_type_smoke_test.cpp
    builtin_function_caller2_test.cpp
    double_type_smoke_test.cpp
    expression_evaluation_visitor_test.cpp
    instance_value_variant_test.cpp
    int_type_smoke_test.cpp
    list_type_smoke_test.cpp
    main.cpp
    reference_smoke_tests.cpp
    smoke_test_fixture.hpp
    string_type_smoke_tests.cpp
    version_type_smoke_tests.cpp
    mock/cmake_facade_mock.hpp
    mock/function_caller_mock.hpp
    mock/identifiers_context_mock.hpp
    mock/instance_mock.hpp
    mock/instances_holder_mock.hpp
)


if(NOT ${CMAKE_VERSION} VERSION_LESS "3.9.0") 
    source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} )
endif()


add_executable(${TESTS_NAME} ${SOURCES} )
target_link_libraries(${TESTS_NAME} gmock_main  exec  lexer  ast sema errors_observer_mock tests_common)
target_include_directories(${TESTS_NAME} PRIVATE ${CMAKESL_SOURCES_DIR} ${CMAKESL_DIR})
add_test(NAME ${TESTS_NAME} COMMAND ${TESTS_NAME}) 
