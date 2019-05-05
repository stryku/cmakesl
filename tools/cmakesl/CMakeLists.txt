set(CMSL_EXECUTABLE_SOURCES
    main.cpp
)

add_executable(cmakesl ${CMSL_EXECUTABLE_SOURCES})

target_include_directories(cmakesl
    PRIVATE
        ${CMAKESL_SOURCES_DIR}
        ${CMAKESL_FACADE_DIR}
)

target_link_libraries(cmakesl 
    PRIVATE
        exec
        sema
        errors
)

install(TARGETS cmakesl DESTINATION bin)
