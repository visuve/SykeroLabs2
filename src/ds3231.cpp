#include "ds3231.hpp"

#include <cstdio>

#include <pico/stdlib.h>
#include <pico/util/datetime.h>

constexpr uint8_t from_bcd(uint8_t value) {
	return value - 6 * (value >> 4);
}

constexpr uint8_t to_bcd(uint8_t value) {
	return value + 6 * (value / 10);
}

constexpr uint8_t from_ds3231_dotw(uint8_t dotw) {
	return dotw == 7 ? 0 : dotw;
}

constexpr uint8_t to_ds3231_dotw(uint8_t dotw) {
	return dotw == 0 ? 7 : dotw;
}

ds3231::ds3231(uint sda_pin, uint scl_pin, i2c_inst_t* i2c) :
	_i2c(i2c) {
	i2c_init(i2c, 400000);

	gpio_set_function(sda_pin, GPIO_FUNC_I2C);
	gpio_set_function(scl_pin, GPIO_FUNC_I2C);

	gpio_pull_up(sda_pin);
	gpio_pull_up(scl_pin);
}

datetime_t ds3231::datetime() const {
	uint8_t value = 0x00;
	i2c_write_blocking(_i2c, ADDRESS, &value, 1, true);
	
	uint8_t buffer[7] = {};
	i2c_read_blocking(_i2c, ADDRESS, buffer, 7, false);

	datetime_t result = {};

	result.year = from_bcd(buffer[6]) + 2000;
	result.month = from_bcd(buffer[5] & 0x7F);
	result.day = from_bcd(buffer[4]);
	result.dotw = from_ds3231_dotw(from_bcd(buffer[3]));
	result.hour = from_bcd(buffer[2]);
	result.min = from_bcd(buffer[1]);
	result.sec = from_bcd(buffer[0] & 0x7F);

	return result;
}

void ds3231::adjust(const datetime_t& dt) {
	uint8_t buffer[8] =	{
		0x00,
		to_bcd(dt.sec),
		to_bcd(dt.min),
		to_bcd(dt.hour),
		to_bcd(to_ds3231_dotw(dt.dotw)),
		to_bcd(dt.day),
		to_bcd(dt.month),
		to_bcd(dt.year - 2000)
	};

	i2c_write_blocking(_i2c, ADDRESS, buffer, 8, false);
}

const char* ds3231::timestamp(const datetime_t& dt) const {
	snprintf(_timestamp, 20,
		"%04u-%02u-%02uT%02u:%02u:%02u",
		dt.year,
		dt.month,
		dt.day,
		dt.hour,
		dt.min,
		dt.sec);

	return _timestamp;
}

const char* ds3231::timestamp() const {
	const datetime_t dt = datetime();
	return timestamp(dt);
}