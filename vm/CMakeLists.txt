
add_subdirectory(config)
add_subdirectory(devices)
add_subdirectory(cpu)
add_subdirectory(instructions)
add_subdirectory(vm)

add_executable(my_vm main.cpp)
target_link_libraries(my_vm
    PRIVATE vm_lib vm_config project_options project_warnings)

add_custom_target(generate_test_out
    COMMAND $<TARGET_FILE:my_asm> ${CMAKE_SOURCE_DIR}/testdata/test.code ${CMAKE_SOURCE_DIR}/testdata/out)

add_custom_target(test_run_vm
    COMMAND $<TARGET_FILE:my_asm> ${CMAKE_SOURCE_DIR}/testdata/test.code ${CMAKE_SOURCE_DIR}/testdata/out
    COMMAND $<TARGET_FILE:my_vm> ${CMAKE_SOURCE_DIR}/testdata/out DEPENDS my_vm my_asm)
