set(source_files
    main.cpp

    vm.cpp

    instructions/decode.cpp
    instructions/encode.cpp
    instructions/run.cpp
    instructions/instructions.cpp
    devices/rom.cpp
    devices/ram.cpp
    devices/device.cpp
)

# file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/test.code
# DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
add_executable(${VM_EXE_NAME} ${source_files})
