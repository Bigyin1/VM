

set(vm_name "my_vm")

add_test(NAME consoleTest COMMAND
    go test -timeout 5s
    ${CMAKE_CURRENT_SOURCE_DIR}/console_test.go
    ${CMAKE_CURRENT_SOURCE_DIR}/utils.go
)

add_test(NAME graphicsTest COMMAND
    go test -timeout 5s
    ${CMAKE_CURRENT_SOURCE_DIR}/graphic_test.go
    ${CMAKE_CURRENT_SOURCE_DIR}/utils.go
)

add_test(NAME consoleLinkedTest COMMAND
    go test -timeout 5s
    ${CMAKE_CURRENT_SOURCE_DIR}/console_linked_test.go
    ${CMAKE_CURRENT_SOURCE_DIR}/utils.go
)

add_test(NAME quadEqTest COMMAND
    go test -timeout 5s
    ${CMAKE_CURRENT_SOURCE_DIR}/quadEq_test.go
    ${CMAKE_CURRENT_SOURCE_DIR}/utils.go
)

add_custom_target(end2end_get_exe_files)

add_dependencies(end2end_get_exe_files my_vm my_asm my_ld readobj)

add_custom_command(TARGET end2end_get_exe_files POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E rm -r -f ${CMAKE_CURRENT_SOURCE_DIR}/progs
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_SOURCE_DIR}/progs
    COMMAND ${CMAKE_COMMAND} -E copy
    $<TARGET_FILE:my_asm> ${CMAKE_CURRENT_SOURCE_DIR}/progs
    COMMAND ${CMAKE_COMMAND} -E copy
    $<TARGET_FILE:${vm_name}> ${CMAKE_CURRENT_SOURCE_DIR}/progs
    COMMAND ${CMAKE_COMMAND} -E copy
    $<TARGET_FILE:my_ld> ${CMAKE_CURRENT_SOURCE_DIR}/progs)
