#include <stdio.h>
#include "pico/stdlib.h"

int main() {
	gpio_init(PICO_DEFAULT_LED_PIN);
	gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);

	stdio_init_all();

	while (true) {
		gpio_put(PICO_DEFAULT_LED_PIN, 1);
		sleep_ms(1000);
		gpio_put(PICO_DEFAULT_LED_PIN, 0);
		sleep_ms(1000);
	}

	return 0;
}