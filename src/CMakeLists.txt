set(TARGET_NAME main)

file(GLOB_RECURSE SOURCES "*.cpp" "*.h")

add_executable(${TARGET_NAME}
    ${SOURCES}
    )
target_include_directories(${TARGET_NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/..
    ${CMAKE_SOURCE_DIR}/drivers/
    )
target_link_libraries(${TARGET_NAME} PRIVATE
    #pico_async_context_freertos
    #FreeRTOS-Kernel-Heap4
    pico_stdlib
    pico_cyw43_arch_lwip_threadsafe_background
    )
#target_compile_definitions(${TARGET_NAME} PRIVATE
#    configNUMBER_OF_CORES=1
#    )
# enable usb output, disable uart output
pico_enable_stdio_usb(${TARGET_NAME} 1)
pico_enable_stdio_uart(${TARGET_NAME} 0)
pico_add_extra_outputs(${TARGET_NAME})