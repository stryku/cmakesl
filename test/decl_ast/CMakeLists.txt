include(${CMAKESL_DIR}/cmake/cmsl_cmake_utils.cmake)

cmsl_add_test(
    NAME
        decl_ast
    SOURCES
        parser_test.cpp
    INCLUDE_DIRS
        ${CMAKESL_SOURCES_DIR}
        ${CMAKESL_FACADE_SOURCES_DIR}
        ${CMAKESL_DIR}
    LIBRARIES
        decl_ast
        lexer
        errors_observer_mock
        tests_common
)
