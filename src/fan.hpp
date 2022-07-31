#pragma once

#include "sykerolabs.hpp"

class fan {
public:
	static void tachometer_callback(uint gpio, uint32_t /*events*/);

	fan(uint pwm_pin, uint rpm_pin, float min_temp, float max_temp);

	void adjust(float temperature);
	void set_speed_percent(uint8_t percent);

	uint32_t rpm();

private:
	const uint _pwm_slice;
	const uint _pwm_channel;
	const uint _rpm_pin;
	const float _min_temp;
	const float _max_temp;

	volatile uint8_t _speed_percent = 0;

	inline static volatile uint32_t _revolutions[pins::LAST] = {};
	uint32_t _rpm_last_measure_time = 0;
};