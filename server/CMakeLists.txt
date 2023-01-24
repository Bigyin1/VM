add_custom_target(server_get_exe_files COMMAND)

add_custom_command(TARGET server_get_exe_files POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E rm -r -f ${CMAKE_CURRENT_SOURCE_DIR}/progs
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_SOURCE_DIR}/progs
    COMMAND ${CMAKE_COMMAND} -E copy
    $<TARGET_FILE:my_asm> ${CMAKE_CURRENT_SOURCE_DIR}/progs
    COMMAND ${CMAKE_COMMAND} -E copy
    $<TARGET_FILE:my_vm> ${CMAKE_CURRENT_SOURCE_DIR}/progs)

add_test(NAME serverTest COMMAND
    go test -timeout 5s ${CMAKE_CURRENT_SOURCE_DIR}/.
)
