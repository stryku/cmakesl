set(AST_TEST_SOURCES 
    main.cpp
    mock/ast_node_mock.hpp
)

set(INCLUDE_DIRS
    ${CMAKESL_SOURCES_DIR}
)

set(LIBS
    ast
    lexer
    errors
)

cmsl_add_test(ast
    "${AST_TEST_SOURCES}"
    "${INCLUDE_DIRS}"
    "${LIBS}"
)
