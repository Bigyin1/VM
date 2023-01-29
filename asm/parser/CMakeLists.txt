add_library(asm_parser utils.cpp
    section_parser.cpp parser.cpp symbols.cpp directives.cpp
    command_parser.cpp errors.cpp cmd_postfix_parser.cpp)

target_link_libraries(asm_parser
    PRIVATE instr_encode project_options project_warnings
    PUBLIC instr asm_tokenizer)

target_include_directories(asm_parser PUBLIC includes tmpl PRIVATE .)
