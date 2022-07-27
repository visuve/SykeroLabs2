#pragma once

#include "sykerolabs.hpp"

class fan {
public:
	static void tachometer_callback(uint gpio, uint32_t /*events*/);

	fan(uint pwm_pin, uint rpm_pin);

	void set_speed_percent(uint16_t percent);

	uint32_t rpm();

private:
	uint _pwm_slice = 0;
	uint _pwm_channel = 0;

	uint _rpm_pin = 0;
	static volatile uint32_t _revolutions[pins::LAST];
	uint32_t _rpm_last_measure_time = 0;
};