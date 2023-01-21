add_library(vm_lib vm.cpp loader.cpp init.cpp)

target_link_libraries(vm_lib
    PRIVATE binformat instr_run instr_decode project_options project_warnings
    PUBLIC cpu
)

target_include_directories(vm_lib PUBLIC includes)
