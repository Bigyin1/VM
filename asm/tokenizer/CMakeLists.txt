add_library(asm_tokenizer tokenizer.cpp utils.cpp errors.cpp tockCheckers.cpp)

target_link_libraries(asm_tokenizer
    PRIVATE project_options project_warnings)

target_include_directories(asm_tokenizer PUBLIC includes PRIVATE .)
