#include <stdio.h>
#include <pico/stdlib.h>
#include <hardware/gpio.h>

#include "fan.hpp"
#include "onboard_temperature_sensor.hpp"
#include "ds18b20.hpp"

int main() {
	stdio_init_all();

	gpio_init(pins::ONBOARD_LED);
	gpio_set_dir(pins::ONBOARD_LED, GPIO_OUT);

	fan fan1(pins::FAN1_PWM_OUT, pins::FAN1_RPM_IN);
	fan fan2(pins::FAN2_PWM_OUT, pins::FAN2_RPM_IN);

	onboard_temperature_sensor onboard_temperature;	
	ds18b20 temperature(pins::DS18B20_IN);

	puts("Fan 1 RPM; Fan 2 RPM; Temperature");

	while (true) {
		gpio_put(pins::ONBOARD_LED, 1);
		sleep_ms(1000);
		gpio_put(pins::ONBOARD_LED, 0);
		sleep_ms(1000);

		printf("%u;%u;%.2f;%.2f\n",
			fan1.rpm(),
			fan2.rpm(),
			onboard_temperature.celcius(),
			temperature.celcius());
	}

	return 0;
}