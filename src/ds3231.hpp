#pragma once

#include <pico.h>
#include <hardware/i2c.h>

class ds3231 {
public:
	ds3231(uint sda_pin, uint scl_pin, i2c_inst_t* i2c);

	datetime_t datetime() const;
	void adjust(const datetime_t& dt);

	const char* timestamp(const datetime_t& dt) const;
	const char* timestamp() const;

private:
	i2c_inst_t* _i2c;
	mutable char _timestamp[20] = {};

	static constexpr uint8_t ADDRESS = 0x68;
};