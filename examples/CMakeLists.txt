project(examples)

include_directories(${CMAKESL_SOURCES_DIR})

add_executable(complete complete.c)
target_link_libraries(complete cmsl_tools)
target_include_directories(complete PRIVATE ${CMAKESL_DIR}/tools)

add_executable(index index.c)
target_link_libraries(index cmsl_tools)
target_include_directories(index PRIVATE ${CMAKESL_DIR}/tools)
