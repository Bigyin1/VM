
install(TARGETS my_asm my_ld my_vm readobj DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/progs)

set(STDLIB_SRCS linear printStr quad quadEqSolver drawCircle)

foreach(SRC ${STDLIB_SRCS})
    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/stdlib/bin/${SRC}.o
        DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/stdlib/${SRC}.code
        ${CMAKE_CURRENT_SOURCE_DIR}/stdlib/bin
        $<TARGET_FILE:my_asm>

        COMMAND $<TARGET_FILE:my_asm>
        ${CMAKE_CURRENT_SOURCE_DIR}/stdlib/${SRC}.code
        ${CMAKE_CURRENT_SOURCE_DIR}/stdlib/bin/${SRC}.o
    )

    list(APPEND STDLIB_OBJS ${CMAKE_CURRENT_SOURCE_DIR}/stdlib/bin/${SRC}.o)
endforeach()

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/stdlib/bin
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_SOURCE_DIR}/stdlib/bin
)

add_custom_target(BuildServerStdLib
    DEPENDS ${STDLIB_OBJS}
)

add_test(NAME serverTest COMMAND
    go test -timeout 5s ${CMAKE_CURRENT_SOURCE_DIR}/.
)
