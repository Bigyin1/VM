add_executable(my_ld main.cpp cmdargs.cpp ld.cpp ld.hpp utils.cpp link.cpp)
target_link_libraries(my_ld
    PRIVATE binformat
    project_options project_warnings)
