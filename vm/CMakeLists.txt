set(source_files
    main.cpp

    vm.cpp

    instructions/decode.cpp
    instructions/encode.cpp
    instructions/run.cpp
    instructions/instructions.cpp
    instructions/argument.cpp
    instructions/registers/registers.cpp
    devices/rom.cpp
    devices/ram.cpp
    devices/console.cpp
    devices/device.cpp
)

add_executable(${VM_EXE_NAME} ${source_files})
