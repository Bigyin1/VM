
add_subdirectory(tokenizer)
add_subdirectory(parser)
add_subdirectory(encoder)
add_subdirectory(asm)

add_executable(my_asm main.cpp)
target_link_libraries(my_asm
    PRIVATE asm_lib project_options project_warnings)
