#include "ds18b20.hpp"

#include <cstdio>

#include <pico/time.h>
#include <hardware/gpio.h>

namespace onewire {
	enum command : uint8_t {
		READ_ROM = 0x33,
		CONVERT_TEMPERATURE = 0x44,
		READ_SCRATCHPAD = 0xBE,
		SKIP_ROM = 0xCC
	};
}

constexpr uint8_t DALLAS_CRC_LOOKUP_TABLE[] = {
	0X00, 0X5E, 0XBC, 0XE2, 0X61, 0X3F, 0XDD, 0X83,
	0XC2, 0X9C, 0X7E, 0X20, 0XA3, 0XFD, 0X1F, 0X41,
	0X9D, 0XC3, 0X21, 0X7F, 0XFC, 0XA2, 0X40, 0X1E,
	0X5F, 0X01, 0XE3, 0XBD, 0X3E, 0X60, 0X82, 0XDC,
	0X23, 0X7D, 0X9F, 0XC1, 0X42, 0X1C, 0XFE, 0XA0,
	0XE1, 0XBF, 0X5D, 0X03, 0X80, 0XDE, 0X3C, 0X62,
	0XBE, 0XE0, 0X02, 0X5C, 0XDF, 0X81, 0X63, 0X3D,
	0X7C, 0X22, 0XC0, 0X9E, 0X1D, 0X43, 0XA1, 0XFF,
	0X46, 0X18, 0XFA, 0XA4, 0X27, 0X79, 0X9B, 0XC5,
	0X84, 0XDA, 0X38, 0X66, 0XE5, 0XBB, 0X59, 0X07,
	0XDB, 0X85, 0X67, 0X39, 0XBA, 0XE4, 0X06, 0X58,
	0X19, 0X47, 0XA5, 0XFB, 0X78, 0X26, 0XC4, 0X9A,
	0X65, 0X3B, 0XD9, 0X87, 0X04, 0X5A, 0XB8, 0XE6,
	0XA7, 0XF9, 0X1B, 0X45, 0XC6, 0X98, 0X7A, 0X24,
	0XF8, 0XA6, 0X44, 0X1A, 0X99, 0XC7, 0X25, 0X7B,
	0X3A, 0X64, 0X86, 0XD8, 0X5B, 0X05, 0XE7, 0XB9,
	0X8C, 0XD2, 0X30, 0X6E, 0XED, 0XB3, 0X51, 0X0F,
	0X4E, 0X10, 0XF2, 0XAC, 0X2F, 0X71, 0X93, 0XCD,
	0X11, 0X4F, 0XAD, 0XF3, 0X70, 0X2E, 0XCC, 0X92,
	0XD3, 0X8D, 0X6F, 0X31, 0XB2, 0XEC, 0X0E, 0X50,
	0XAF, 0XF1, 0X13, 0X4D, 0XCE, 0X90, 0X72, 0X2C,
	0X6D, 0X33, 0XD1, 0X8F, 0X0C, 0X52, 0XB0, 0XEE,
	0X32, 0X6C, 0X8E, 0XD0, 0X53, 0X0D, 0XEF, 0XB1,
	0XF0, 0XAE, 0X4C, 0X12, 0X91, 0XCF, 0X2D, 0X73,
	0XCA, 0X94, 0X76, 0X28, 0XAB, 0XF5, 0X17, 0X49,
	0X08, 0X56, 0XB4, 0XEA, 0X69, 0X37, 0XD5, 0X8B,
	0X57, 0X09, 0XEB, 0XB5, 0X36, 0X68, 0X8A, 0XD4,
	0X95, 0XCB, 0X29, 0X77, 0XF4, 0XAA, 0X48, 0X16,
	0XE9, 0XB7, 0X55, 0X0B, 0X88, 0XD6, 0X34, 0X6A,
	0X2B, 0X75, 0X97, 0XC9, 0X4A, 0X14, 0XF6, 0XA8,
	0X74, 0X2A, 0XC8, 0X96, 0X15, 0X4B, 0XA9, 0XF7,
	0XB6, 0XE8, 0X0A, 0X54, 0XD7, 0X89, 0X6B, 0X35
};

ds18b20::ds18b20(uint pin) :
	_pin(pin) {
	gpio_init(_pin);
}

bool ds18b20::reset() const {
	gpio_set_dir(_pin, GPIO_OUT);
	gpio_put(_pin, false);
	busy_wait_us(480);
	gpio_set_dir(_pin, GPIO_IN);
	busy_wait_us(70);
	bool result = !gpio_get(_pin);
	busy_wait_us(410);

	if (!result) {
		puts("reset failed!");
	}

	return result;
}

void ds18b20::write_bit(bool value) const {
	gpio_set_dir(_pin, GPIO_OUT);
	gpio_put(_pin, false);
	busy_wait_us(7);

	if (value) {
		gpio_put(_pin, true);
		busy_wait_us(55);
	} else {
		busy_wait_us(90);
		gpio_put(_pin, true);
		busy_wait_us(5);
	}
}

void ds18b20::write_byte(uint8_t value) const {
	write_bit(value >> 0 & 1);
	write_bit(value >> 1 & 1);
	write_bit(value >> 2 & 1);
	write_bit(value >> 3 & 1);
	write_bit(value >> 4 & 1);
	write_bit(value >> 5 & 1);
	write_bit(value >> 6 & 1);
	write_bit(value >> 7 & 1);
}

bool ds18b20::read_bit() const {
	gpio_set_dir(_pin, GPIO_OUT);
	gpio_put(_pin, false);
	busy_wait_us(7);
	gpio_set_dir(_pin, GPIO_IN);
	busy_wait_us(7);
	bool result = gpio_get(_pin);
	busy_wait_us(45);
	return result;
}

uint8_t ds18b20::read_byte() const {
	return read_bit() << 0 |
		read_bit() << 1 |
		read_bit() << 2 |
		read_bit() << 3 |
		read_bit() << 4 |
		read_bit() << 5 |
		read_bit() << 6 |
		read_bit() << 7;
}

void ds18b20::convert_temperature() {
	reset();
	write_byte(onewire::command::SKIP_ROM);
	write_byte(onewire::command::CONVERT_TEMPERATURE);
	busy_wait_ms(750);
}

void ds18b20::read_scratchpad() {
	reset();
	write_byte(onewire::command::SKIP_ROM);
	write_byte(onewire::command::READ_SCRATCHPAD);

	for (uint8_t i = 0; i < 9; ++i) {
		_scratchpad[i] = read_byte();
	}
}

bool ds18b20::is_scratchpad_checksum_valid() {
	uint8_t crc = 0;

	for (uint8_t i = 0; i < 8; ++i) {
		crc = DALLAS_CRC_LOOKUP_TABLE[_scratchpad[i] ^ crc];
	}

	if (crc != _scratchpad[8]) {
		puts("checksum mismatch!");
		return false;
	}

	return true;
}

float ds18b20::celcius() {
	convert_temperature();
	read_scratchpad();

	if (!is_scratchpad_checksum_valid()) {
		return -273.15f;
	}

	int reading = (_scratchpad[1] << 8) + _scratchpad[0];

	if (reading & 0x8000) {
		reading = 0 - ((reading ^ 0xffff) + 1);
	}

	return reading / 16.0f;
}