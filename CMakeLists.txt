cmake_minimum_required(VERSION 3.17)
project(VM VERSION 0.1 LANGUAGES CXX)

include(cmake/CompileOptions.cmake)

enable_testing()

add_subdirectory(binformat)
add_subdirectory(vm)
add_subdirectory(asm)
add_subdirectory(tests)
add_subdirectory(server)
