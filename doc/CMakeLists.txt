find_package(Doxygen)
if (DOXYGEN_FOUND)
    set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
    set(CONFIGURED_COXYGEN_CONFIG_FILE ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
    set(DOXYGEN_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/doc)
    set(DOXYGEN_HTML_OUT ${DOXYGEN_OUTPUT_DIR}/html)

    configure_file(${DOXYGEN_IN} ${CONFIGURED_COXYGEN_CONFIG_FILE} @ONLY)

    add_custom_target(cmakesl_doc ALL
        COMMAND
            ${DOXYGEN_EXECUTABLE} ${CONFIGURED_COXYGEN_CONFIG_FILE}
        WORKING_DIRECTORY
            ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT
            "Generating API documentation with Doxygen"
        VERBATIM
    )

    install(
        DIRECTORY
            ${DOXYGEN_HTML_OUT}
        DESTINATION
            doc/cmakesl
    )

else ()
    message("Doxygen need to be installed to generate the doxygen documentation")
endif ()

install(
    DIRECTORY
        builtin
    DESTINATION
        doc/cmakesl
    FILES_MATCHING PATTERN
        "*.cmsl"
)
