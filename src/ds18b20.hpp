#pragma once

#include <pico.h>

// https://www.maximintegrated.com/en/design/technical-documents/app-notes/1/162.html
class ds18b20 {
public:
	ds18b20(uint pin);

	float celcius();

private:
	bool reset() const;

	void write_bit(bool bit_data) const;
	void write_byte(uint8_t data) const;

	bool read_bit() const;
	uint8_t read_byte() const;

	void convert_temperature();
	void read_scratchpad();
	bool is_scratchpad_checksum_valid();

	uint _pin;
	uint8_t _scratchpad[9] = {};
};