add_library(asm_encoder asmencoder.cpp)

target_link_libraries(asm_encoder
    PRIVATE instr_encode project_options project_warnings
    PUBLIC binformat asm_parser)

target_include_directories(asm_encoder PUBLIC includes)
