add_library(asm_encoder encoder.cpp)

target_link_libraries(asm_encoder
    PRIVATE binformat instr_encode project_options project_warnings
    PUBLIC asm_parser)

target_include_directories(asm_encoder PUBLIC includes)
