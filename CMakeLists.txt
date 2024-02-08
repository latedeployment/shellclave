cmake_minimum_required (VERSION 3.0)
project (shellclave)
set(CMAKE_C_FLAGS "-Wall")
set(DEBUG_PROCESS_NAME "slave_example")

configure_file(shellclave_config.h.in ${CMAKE_SOURCE_DIR}/shellclave_config.h)

set(SHELLCLAVE_LIB_SRC shellclave_lib.c shellclave_utils.c shellclave_ptrace_linux.c shellclave_cli.c )
set(SLAVEEXAMPLE_SRC slave_example.c)

add_library(shellclave_lib STATIC ${SHELLCLAVE_LIB_SRC})

add_executable(shellclave shellclave.c)
target_link_libraries(shellclave shellclave_lib seccomp)
add_executable(${DEBUG_PROCESS_NAME} ${SLAVEEXAMPLE_SRC})

target_link_libraries(shellclave readline)

# format (astyle) the source code
add_custom_target(format COMMAND ./style_src.sh 
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}) 