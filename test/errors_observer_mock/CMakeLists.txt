set(ERRORS_OBSERVER_MOCK_SOURCES
    errors_observer_mock.cpp
)

add_library(errors_observer_mock "${ERRORS_OBSERVER_MOCK_SOURCES}")
target_include_directories(errors_observer_mock
    PRIVATE
        ${CMAKESL_SOURCES_DIR}
        ${CMAKESL_TESTS_DIR}
        "${gtest_SOURCE_DIR}/include"
        "${gmock_SOURCE_DIR}/include"
)
