cmake_minimum_required(VERSION 3.13)

# The Pico SDK is deliberately kept outside this repository. Configure with
# -DPICO_SDK_PATH=/absolute/path/to/pico-sdk.
if (NOT DEFINED PICO_SDK_PATH)
    message(FATAL_ERROR "Set PICO_SDK_PATH to a Pico SDK checkout. See README.md.")
endif()

if (NOT DEFINED PICO_BOARD)
    set(PICO_BOARD waveshare_rp2040_zero CACHE STRING "Target board")
endif()

include(${PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(rp2040_zero_uart_adapter C CXX ASM)

pico_sdk_init()

add_executable(rp2040_zero_uart_adapter
    src/main.cpp
    src/usb_descriptors.c
)

target_include_directories(rp2040_zero_uart_adapter PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/src
)

target_link_libraries(rp2040_zero_uart_adapter PRIVATE
    pico_stdlib
    pico_unique_id
    hardware_uart
    hardware_pio
    tinyusb_device
    tinyusb_board
)

# TinyUSB uses the Pico SDK's FreeRTOS-free integration.
target_compile_definitions(rp2040_zero_uart_adapter PRIVATE
    CFG_TUSB_OS=OPT_OS_PICO
)

pico_generate_pio_header(rp2040_zero_uart_adapter
    ${CMAKE_CURRENT_LIST_DIR}/src/ws2812.pio
)

# CDC interface 0 is wired to the SDK stdio implementation. It is exclusively
# for diagnostics; CDC interface 1 is the UART data channel.
pico_enable_stdio_usb(rp2040_zero_uart_adapter 1)
pico_enable_stdio_uart(rp2040_zero_uart_adapter 0)
pico_add_extra_outputs(rp2040_zero_uart_adapter)
