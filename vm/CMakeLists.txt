
add_subdirectory(devices)
add_subdirectory(cpu)
add_subdirectory(instructions)
add_subdirectory(vm)

add_executable(my_vm main.cpp)
target_link_libraries(my_vm
    PRIVATE vm_lib project_options project_warnings)
