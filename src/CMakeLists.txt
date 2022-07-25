add_executable(SykeroLabs "Main.cpp")

target_link_libraries(SykeroLabs pico_stdlib)

pico_enable_stdio_usb(SykeroLabs 1)
pico_enable_stdio_uart(SykeroLabs 0)

pico_add_extra_outputs(SykeroLabs)
