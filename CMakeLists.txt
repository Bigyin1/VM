cmake_minimum_required(VERSION 3.17)
project(VM VERSION 0.1 LANGUAGES CXX)

include(cmake/CompileOptions.cmake)
include(cmake/Doxygen.cmake)

enable_testing()

add_subdirectory(binformat)
add_subdirectory(vm)
add_subdirectory(asm)
add_subdirectory(ld)
add_subdirectory(readobj)
add_subdirectory(tests)
add_subdirectory(server)
