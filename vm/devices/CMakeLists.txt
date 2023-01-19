
add_library(devices device.cpp console.cpp ram.cpp rom.cpp)
target_link_libraries(devices
    PRIVATE project_options project_warnings
    PUBLIC instr)

target_include_directories(devices PUBLIC includes)
