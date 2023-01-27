add_executable(vmTest vm_test.cpp)

target_link_libraries(vmTest PRIVATE
    instr_run instr vm_lib vm_config
    project_options project_warnings)

add_test(NAME vmTest COMMAND vmTest)
