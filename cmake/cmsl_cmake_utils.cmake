function(cmsl_add_test NAME SOURCES INCLUDE_DIRS LIBRARIES_TO_LINK)
    set(test_name ${NAME}_test)

    add_executable(${test_name} ${SOURCES})

    target_link_libraries(${test_name}
        PRIVATE
            gmock_main
            ${LIBRARIES_TO_LINK}
    )

    target_include_directories(${test_name}
        PRIVATE
            ${INCLUDE_DIRS}
    )

    add_test(NAME ${test_name} COMMAND ${test_name})
endfunction()
