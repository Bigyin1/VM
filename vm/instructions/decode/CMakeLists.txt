add_library(instr_decode decode.cpp)
target_link_libraries(instr_decode
    PRIVATE project_options project_warnings
    PUBLIC instr)

target_include_directories(instr_decode PUBLIC .)
