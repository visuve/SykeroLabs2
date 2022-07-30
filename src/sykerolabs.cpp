#include <stdio.h>
#include <pico/stdlib.h>
#include <hardware/gpio.h>

#include "fan.hpp"
#include "onboard_temperature_sensor.hpp"
#include "ds18b20.hpp"
#include "ds3231.hpp"
#include "relay.hpp"

bool is_night_time(const datetime_t& dt) {
	return dt.hour >= 21 || dt.hour <= 8;
}

bool fan_toggle_condition(void** data) {
	auto rtc = reinterpret_cast<ds3231*>(data[0]);

	const datetime_t dt = rtc->datetime();

	if (is_night_time(dt)) {
		return false;
	}

	auto temperature_sensor = reinterpret_cast<ds18b20*>(data[1]);

	if (temperature_sensor->celcius() <= 20.00f) {
		return false;
	}

	return true;
}

void adjust(ds3231& rtc) {
	datetime_t dt;
	dt.year = 2022;
	dt.month = 7;
	dt.day = 29;
	dt.dotw = 5;
	dt.hour = 0;
	dt.min = 25;
	dt.sec = 0;
	rtc.adjust(dt);
}

int main() {
	stdio_init_all();

	gpio_init(pins::ONBOARD_LED);
	gpio_set_dir(pins::ONBOARD_LED, GPIO_OUT);

	ds3231 rtc(pins::RTC_I2C_SDA, pins::RTC_I2C_SCL, &i2c0_inst);

	onboard_temperature_sensor onboard_temperature;
	ds18b20 temperature_sensor(pins::DS18B20_IN);

	fan fan1(pins::FAN1_PWM_OUT, pins::FAN1_RPM_IN);
	fan fan2(pins::FAN2_PWM_OUT, pins::FAN2_RPM_IN);

	void* fan_toggle_data[2] = { &rtc, &temperature_sensor };
	relay fan_relay(pins::RELAY2_OUT, fan_toggle_condition, fan_toggle_data, 60000);

	puts("Time;Temperature 1; Temperature 2;Fan 1 RPM;Fan 2 RPM");

	while (true) {
		gpio_put(pins::ONBOARD_LED, true);
		sleep_ms(2500);

		gpio_put(pins::ONBOARD_LED, false);
		sleep_ms(2500);

		printf("%s;%.2f;%.2f;%u;%u\n",
			rtc.timestamp(),
			onboard_temperature.celcius(),
			temperature_sensor.celcius(),
			fan1.rpm(),
			fan2.rpm());
	}
	return 0;
}