add_library(instr_run run.cpp)

target_link_libraries(instr_run
    PRIVATE project_options project_warnings
    PUBLIC cpu instr)

target_include_directories(instr_run PUBLIC .)
