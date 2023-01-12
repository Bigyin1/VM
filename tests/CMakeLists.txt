set(source_files

    # main.cpp
    decEnc_test.cpp

    ../asm/utils.cpp
    ../asm/assemble.cpp
    ../asm/tokenizer/tokenizer.cpp

    ../asm/parser/parser.cpp
    ../asm/parser/utils.cpp
    ../asm/parser/labels.cpp
    ../asm/parser/directives.cpp
    ../asm/parser/section_parser.cpp
    ../asm/parser/command_parser.cpp

    ../vm/instructions/decode.cpp
    ../vm/instructions/encode.cpp
    ../vm/instructions/run.cpp
    ../vm/instructions/instructions.cpp
    ../vm/instructions/argument.cpp
    ../vm/instructions/registers/registers.cpp
    ../vm/devices/device.cpp

    ../asm/assembler/assembler.cpp
)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test.code ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)

add_executable(${TESTS_EXE_NAME} ${source_files})
