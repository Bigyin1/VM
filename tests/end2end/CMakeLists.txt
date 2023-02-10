

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

install(TARGETS my_asm my_ld my_vm DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/progs)
