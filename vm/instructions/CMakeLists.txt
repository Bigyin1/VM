add_library(instr argument.cpp instructions.cpp registers.cpp)

target_link_libraries(instr
    PRIVATE project_options project_warnings)

target_include_directories(instr PUBLIC includes tmpl)

add_subdirectory(run)
add_subdirectory(decode)
add_subdirectory(encode)
