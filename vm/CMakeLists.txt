set(source_files
    main.cpp

    instructions/decode.cpp
    instructions/encode.cpp
    instructions/instructions.cpp
)

# file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/testdata
# DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
add_executable(${VM_EXE_NAME} ${source_files})
