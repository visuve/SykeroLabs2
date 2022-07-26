add_executable(sykerolabs "Main.cpp")

pico_enable_stdio_usb(sykerolabs 1)
pico_enable_stdio_uart(sykerolabs 0)

target_link_libraries(sykerolabs pico_stdlib hardware_pwm)

pico_add_extra_outputs(sykerolabs)
