

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test.code ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)

add_executable(tests decEnc_test.cpp)
target_link_libraries(tests PRIVATE asm_lib instr_decode asm_encoder)
