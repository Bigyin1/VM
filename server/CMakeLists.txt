add_custom_target(fetch_server_files COMMAND)

add_custom_command(TARGET fetch_server_files POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    $<TARGET_FILE:my_asm> ${CMAKE_CURRENT_SOURCE_DIR}/progs
    COMMAND ${CMAKE_COMMAND} -E copy
    $<TARGET_FILE:my_vm> ${CMAKE_CURRENT_SOURCE_DIR}/progs)
