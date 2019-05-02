function(cmsl_add_library NAME SOURCES)
    add_library(${NAME} ${SOURCES})

    if(NOT ${CMAKE_VERSION} VERSION_LESS "3.9.0")
#        source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX ${NAME} FILES ${SOURCES})
    endif()
endfunction()

function(cmsl_add_test NAME SOURCES INCLUDE_DIRS LIBRARIES_TO_LINK)
    if(NOT ${CMAKE_VERSION} VERSION_LESS "3.9.0")
        source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})
    endif()

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
