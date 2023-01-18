add_library(instr_encode encode.cpp)

target_link_libraries(instr_encode
    PRIVATE project_options project_warnings
    PUBLIC instr)

target_include_directories(instr_encode PUBLIC .)
