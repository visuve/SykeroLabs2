file(GLOB sykerolabs_src "*.cpp")

add_executable(sykerolabs ${sykerolabs_src})

pico_enable_stdio_usb(sykerolabs 1)
pico_enable_stdio_uart(sykerolabs 0)

target_precompile_headers(sykerolabs PRIVATE "pch.hpp")
target_link_libraries(sykerolabs pico_stdlib hardware_pwm hardware_adc hardware_i2c)

pico_add_extra_outputs(sykerolabs)
