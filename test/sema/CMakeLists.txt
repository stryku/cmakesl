set(CMAKESL_TEST_SOURCES
    main.cpp
    mock/add_subdirectory_semantic_handler_mock.hpp
    mock/expression_node_mock.hpp
    mock/identifiers_context_mock.hpp
    mock/sema_context_mock.hpp
    mock/sema_function_mock.hpp
    identifiers_context_test.cpp
    overload_resolution_test.cpp
    sema_builder_ast_visitor.cpp
    vatiable_initialization_check_test.cpp
)

set(INCLUDE_DIRS
    ${CMAKESL_SOURCES_DIR}
    ${CMAKESL_TESTS_DIR}
    ${CMAKESL_DIR}
)

set(LIBS
    lexer
    ast
    sema
    errors_observer_mock
    tests_common
)

cmsl_add_test(sema
    "${CMAKESL_TEST_SOURCES}"
    "${INCLUDE_DIRS}"
    "${LIBS}"
)
