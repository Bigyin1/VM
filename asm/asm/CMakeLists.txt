add_library(asm_lib assemble.cpp utils.cpp)

target_link_libraries(asm_lib
    PRIVATE asm_encoder project_options project_warnings
)

target_include_directories(asm_lib PUBLIC includes PRIVATE .)
