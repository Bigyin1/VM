set(source_files
    main.cpp
    utils.cpp
    assemble.cpp
    tokenizer/tokenizer.cpp

    parser/parser.cpp
    parser/utils.cpp
    parser/labels.cpp

    ../vm/instructions/decode.cpp
    ../vm/instructions/encode.cpp
    ../vm/instructions/run.cpp
    ../vm/instructions/instructions.cpp
    ../vm/instructions/argument.cpp
    ../vm/instructions/registers/registers.cpp

    ../vm/devices/device.cpp

    assembler/assembler.cpp
)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/testdata/test.code ${CMAKE_CURRENT_BINARY_DIR}/testdata COPYONLY)

add_executable(${ASM_EXE_NAME} ${source_files})
