

add_library(vm_config config.cpp)
target_link_libraries(vm_config
    PRIVATE project_options project_warnings
    PUBLIC vm_lib devices)

target_include_directories(vm_config PUBLIC .)

include(${CMAKE_SOURCE_DIR}/configs/VMConf.cmake)
include(${CMAKE_SOURCE_DIR}/configs/MajesticConsoleConf.cmake)
include(${CMAKE_SOURCE_DIR}/configs/RamConf.cmake)
include(${CMAKE_SOURCE_DIR}/configs/RomConf.cmake)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.cpp.in
    ${CMAKE_CURRENT_SOURCE_DIR}/config.cpp)
