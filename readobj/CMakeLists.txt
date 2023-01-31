add_executable(readobj main.cpp print_headers.cpp readobj.cpp print_loadable.cpp print_relocs.cpp print_symtab.cpp)
target_link_libraries(readobj
    PUBLIC instr_decode binformat asm_parser
    project_options project_warnings)
