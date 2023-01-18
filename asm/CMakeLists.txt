configure_file(${CMAKE_CURRENT_SOURCE_DIR}/testdata/test.code ${CMAKE_CURRENT_BINARY_DIR}/testdata COPYONLY)

add_subdirectory(tokenizer)
add_subdirectory(parser)
add_subdirectory(encoder)
add_subdirectory(asm)

add_executable(asm main.cpp)
target_link_libraries(asm
    PRIVATE asm_lib project_options project_warnings)
