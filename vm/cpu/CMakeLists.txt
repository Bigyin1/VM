add_library(cpu INTERFACE)
target_link_libraries(cpu INTERFACE devices)

target_include_directories(cpu INTERFACE includes)
