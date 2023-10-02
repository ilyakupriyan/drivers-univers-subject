project(module VERSION 0.0.1 LANGUAGES C)
set (CMAKE_C_STANDART 11)
set (CMAKE_C_STANDART_REQUIRED ON)

# Module info
add_definitions(-D__KERNEL__ -DMODULE)

#Find the kernel release
execute_process(
    COMMAND uname -r
    OUTPUT_VARIABLE KERNEL_RELEASE
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

#Find the headers
find_path (
    KERNEL_HEADERS_DIR
    include/linux/init.h
    PATHS /usr/src/linux-headers-${KERNEL_RELEASE}
)

message(STATUS "Kernel release: ${KERNEL_RELEASE}")
message(STATUS "Kernel headers: ${KERNEL_HEADERS_DIR}")

#include header files
include_directories(
    ${KERNEL_HEADERS_DIR}/include
)

set(DRIVER_FILE file-driver.ko)

set(KBUILD_CMD ${CMAKE_MAKE_PROGRAM} -C ${KERNEL_HEADERS_DIR} modules M=${CMAKE_CURRENT_BINARY_DIR} src=${CMAKE_CURRENT_SOURCE_DIR})

#generate the Kbuild file
file (WRITE ${CMAKE_CURRENT_SOURCE_DIR}/Kbuild "obj-m := file-driver.o")

add_custom_command(OUTPUT ${DRIVER_FILE}
        COMMAND ${KBUILD_CMD}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        DEPENDS file-driver.c VERBATIM
        COMMENT "Building file-driver.ko"
)

add_custom_target(module ALL DEPENDS ${DRIVER_FILE})