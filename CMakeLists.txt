cmake_minimum_required(VERSION 3.13)

# initialize the SDK based on PICO_SDK_PATH
# note: this must happen before project()
include(pico_sdk_import.cmake)

project(my_project)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

# Include all src files
file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.h")

# rest of your project
add_executable(main
    ${SOURCES}
)

# Include lwiopts
target_include_directories(main PRIVATE drivers/)

# Add pico_stdlib library which aggregates commonly used features
target_link_libraries(main pico_stdlib pico_cyw43_arch_lwip_threadsafe_background)

# create map/bin/hex/uf2 file in addition to ELF.
pico_add_extra_outputs(main)

# Enable USB Serial support (optional, if you're using USB)
pico_enable_stdio_usb(main 1)

# Or enable UART (if using hardware serial)
# pico_enable_stdio_uart(hello_world 1)
