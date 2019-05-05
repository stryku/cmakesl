set(SOURCE_LOCATION_MANIPULATOR_TEST_SOURCES 
    main.cpp
    source_location_test.cpp
)

set(INCLUDE_DIRS
    ${CMAKESL_SOURCES_DIR}
)

set(LIBS lexer)

cmsl_add_test(source_location_manipulator
    "${SOURCE_LOCATION_MANIPULATOR_TEST_SOURCES}"
    "${INCLUDE_DIRS}"
    "${LIBS}"
)

