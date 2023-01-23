

add_test(NAME numConsoleTest COMMAND
    go run
    ${CMAKE_CURRENT_SOURCE_DIR}/consTest.go
    $<TARGET_FILE:my_asm>
    $<TARGET_FILE:my_vm>
    ${CMAKE_CURRENT_SOURCE_DIR}/test.code
    ${CMAKE_CURRENT_SOURCE_DIR}/out)
