set(TARGET_NAME main)

file(GLOB_RECURSE SOURCES "*.cpp" "*.h")

set(PICO_LWIP_CONTRIB_PATH ${PICO_SDK_PATH}/lib/lwip/contrib)

message("PICO_LWIP_CONTRIB_PATH = ${PICO_LWIP_CONTRIB_PATH}")

add_executable(${TARGET_NAME}
    ${SOURCES}
    ${PICO_LWIP_CONTRIB_PATH}/apps/ping/ping.c #pico-sdk ping
    )
target_compile_definitions(main PRIVATE #SYS mode, multithreaded
    WIFI_SSID=\"${WIFI_SSID}\"
    WIFI_PASSWORD=\"${WIFI_PASSWORD}\"
    NO_SYS=0            # don't want NO_SYS (generally this would be in your lwipopts.h)
    LWIP_SOCKET=1       # we need the socket API (generally this would be in your lwipopts.h)
    PING_USE_SOCKETS=1
    )
target_include_directories(${TARGET_NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/..
    ${CMAKE_SOURCE_DIR}/drivers/
    ${PICO_LWIP_CONTRIB_PATH}/apps/ping/
    )
target_link_libraries(${TARGET_NAME} PRIVATE
    FreeRTOS-Kernel-Heap4
    pico_stdlib
    pico_cyw43_arch_lwip_sys_freertos
    )

get_target_property(INCLUDE_DIRS ${TARGET_NAME} INCLUDE_DIRECTORIES)
if(INCLUDE_DIRS)
    message("Include directories for ${TARGET_NAME}:")
    foreach(dir ${INCLUDE_DIRS})
        message("  - ${dir}")
    endforeach()
else()
    message("No include directories found for ${TARGET_NAME}")
endif()
#target_compile_definitions(${TARGET_NAME} PRIVATE
#    configNUMBER_OF_CORES=1
#    )
# enable usb output, disable uart output
pico_enable_stdio_usb(${TARGET_NAME} 1)
pico_enable_stdio_uart(${TARGET_NAME} 0)
pico_add_extra_outputs(${TARGET_NAME})