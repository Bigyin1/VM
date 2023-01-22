add_executable(decEncTest decEnc_test.cpp)

target_link_libraries(decEncTest PRIVATE binformat asm_lib instr_decode asm_encoder)

add_test(NAME decEncTest COMMAND decEncTest ${CMAKE_CURRENT_SOURCE_DIR}/test.code)
