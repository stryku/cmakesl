function(cmsl_add_test)
    set(options)
    set(oneValueArgs NAME)
    set(multiValueArgs SOURCES INCLUDE_DIRS LIBRARIES)
    cmake_parse_arguments(CMSL_ADD_TEST "${options}" "${oneValueArgs}"
            "${multiValueArgs}" ${ARGN} )

    set(test_name ${CMSL_ADD_TEST_NAME}_cmakesl_test)

    add_executable(${test_name} ${CMSL_ADD_TEST_SOURCES})

    target_link_libraries(${test_name}
        PRIVATE
            gmock_main
            ${CMSL_ADD_TEST_LIBRARIES}
    )

    target_include_directories(${test_name}
        PRIVATE
            ${CMSL_ADD_TEST_INCLUDE_DIRS}
    )

    target_compile_options(${test_name}
        PRIVATE
            ${CMAKESL_ADDITIONAL_COMPILER_FLAGS}
    )

    add_test(NAME ${test_name} COMMAND ${test_name})
endfunction()
