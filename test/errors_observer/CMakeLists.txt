include(${CMAKESL_DIR}/cmake/cmsl_cmake_utils.cmake)

cmsl_add_test(
    NAME
        errors_observer
    SOURCES
        errors_observer.cpp
    INCLUDE_DIRS
        ${CMAKESL_SOURCES_DIR}
        ${CMAKESL_FACADE_SOURCES_DIR}
        ${CMAKESL_DIR}
    LIBRARIES
        errors
)
