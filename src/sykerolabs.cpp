#include <stdio.h>
#include <pico/stdlib.h>
#include <hardware/gpio.h>

#include "fan.hpp"
#include "onboard_temperature_sensor.hpp"
#include "ds18b20.hpp"
#include "ds3231.hpp"
#include "relay.hpp"

constexpr float TEMPERATURE_MIN = 20.00f;
constexpr float TEMPERATURE_MAX = 40.00f;

bool is_night_time(const datetime_t& dt) {
	return dt.hour >= 21 || dt.hour <= 8;
}

bool pump_toggle_condition(void** data) {
	auto rtc = reinterpret_cast<ds3231*>(data[0]);

	const datetime_t dt = rtc->datetime();

	if (is_night_time(dt)) {
		return false;
	}
	
	auto counter = reinterpret_cast<uint*>(data[1]);

	// Trigger every 60th time, i.e. for a minute in a hour
	return ++(*counter) % 60 == 0;
}

bool fan_toggle_condition(void** data) {
	auto rtc = reinterpret_cast<ds3231*>(data[0]);

	const datetime_t dt = rtc->datetime();

	if (is_night_time(dt)) {
		return false;
	}

	auto temperature_sensor = reinterpret_cast<ds18b20*>(data[1]);

	if (temperature_sensor->celcius() <= TEMPERATURE_MIN) {
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

	onboard_temperature_sensor internal_temperature_sensor;
	ds18b20 external_temperature_sensor(pins::DS18B20_IN);

	fan fan1(pins::FAN1_PWM_OUT, pins::FAN1_RPM_IN, TEMPERATURE_MIN, TEMPERATURE_MAX);
	fan fan2(pins::FAN2_PWM_OUT, pins::FAN2_RPM_IN, TEMPERATURE_MIN, TEMPERATURE_MAX);

	constexpr int32_t INTERVAL_MS = 60000; // A minute

	uint counter = 0;
	void* pump_toggle_data[2] = { &rtc, &counter };
	relay pump_relay(pins::RELAY1_OUT, pump_toggle_condition, pump_toggle_data, INTERVAL_MS);

	// Add artificial delay to audibly hear the relays trigger at different times
	sleep_ms(1000);

	void* fan_toggle_data[2] = { &rtc, &external_temperature_sensor };
	relay fan_relay(pins::RELAY2_OUT, fan_toggle_condition, fan_toggle_data, INTERVAL_MS);

	sleep_ms(1000);

	puts("Time;Internal temperature;External temperature;Fan 1 RPM;Fan 2 RPM;Pump relay;Fan relay");

	float temperature;

	while (true) {
		{
			temperature = external_temperature_sensor.celcius();

			printf("%s;%.2f;%.2f;%u;%u;%s;%s\n",
				rtc.timestamp(),
				internal_temperature_sensor.celcius(),
				temperature,
				fan1.rpm(),
				fan2.rpm(),
				pump_relay.status(),
				fan_relay.status());
		}

		fan1.adjust(temperature);
		fan1.adjust(temperature);

		sleep_ms(INTERVAL_MS);
	}
	return 0;
}