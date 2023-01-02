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
    ../vm/instructions/instructions.cpp

    assembler/assembler.cpp
)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/testdata
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_executable(${ASM_EXE_NAME} ${source_files})
