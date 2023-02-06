add_custom_target(server_get_exe_files ALL)

add_dependencies(server_get_exe_files my_vm my_asm my_ld readobj)

add_custom_command(TARGET server_get_exe_files POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E rm -r -f ${CMAKE_CURRENT_SOURCE_DIR}/progs
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_SOURCE_DIR}/progs
    COMMAND ${CMAKE_COMMAND} -E copy
    $<TARGET_FILE:my_asm> ${CMAKE_CURRENT_SOURCE_DIR}/progs
    COMMAND ${CMAKE_COMMAND} -E copy
    $<TARGET_FILE:my_vm> ${CMAKE_CURRENT_SOURCE_DIR}/progs
    COMMAND ${CMAKE_COMMAND} -E copy
    $<TARGET_FILE:my_ld> ${CMAKE_CURRENT_SOURCE_DIR}/progs
    COMMAND ${CMAKE_COMMAND} -E copy
    $<TARGET_FILE:readobj> ${CMAKE_CURRENT_SOURCE_DIR}/progs
)

add_custom_command(TARGET server_get_exe_files POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E rm -r -f ${CMAKE_CURRENT_SOURCE_DIR}/stdlib/bin
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_SOURCE_DIR}/stdlib/bin
    COMMAND $<TARGET_FILE:my_asm> ${CMAKE_CURRENT_SOURCE_DIR}/stdlib/linear.code
    ${CMAKE_CURRENT_SOURCE_DIR}/stdlib/bin/linear.o
    COMMAND $<TARGET_FILE:my_asm> ${CMAKE_CURRENT_SOURCE_DIR}/stdlib/printStr.code
    ${CMAKE_CURRENT_SOURCE_DIR}/stdlib/bin/printStr.o
    COMMAND $<TARGET_FILE:my_asm> ${CMAKE_CURRENT_SOURCE_DIR}/stdlib/quad.code
    ${CMAKE_CURRENT_SOURCE_DIR}/stdlib/bin/quad.o
    COMMAND $<TARGET_FILE:my_asm> ${CMAKE_CURRENT_SOURCE_DIR}/stdlib/quadEqSolver.code
    ${CMAKE_CURRENT_SOURCE_DIR}/stdlib/bin/quadEqSolver.o
)

add_test(NAME serverTest COMMAND
    go test -timeout 5s ${CMAKE_CURRENT_SOURCE_DIR}/.
)
