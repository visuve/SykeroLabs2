#include "fan.hpp"

#include <pico/stdlib.h>
#include <hardware/pwm.h>
#include <hardware/gpio.h>

void fan::tachometer_callback(uint gpio, uint32_t) {
	++_revolutions[gpio];
}

fan::fan(uint pwm_pin, uint rpm_pin, float min_temp, float max_temp) :
	_pwm_slice(pwm_gpio_to_slice_num(pwm_pin)),
	_pwm_channel(pwm_gpio_to_channel(pwm_pin)),
	_rpm_pin(rpm_pin),
	_min_temp(min_temp),
	_max_temp(max_temp) {
	gpio_set_function(pwm_pin, GPIO_FUNC_PWM);
	gpio_set_dir(rpm_pin, GPIO_IN);

	pwm_set_wrap(_pwm_slice, 100);
	pwm_set_clkdiv(_pwm_slice, 49.5f); // 25.00khz

	set_speed_percent(_speed_percent);
	pwm_set_enabled(_pwm_slice, true);

	_revolutions[rpm_pin] = 0;

	gpio_set_irq_enabled_with_callback(
		rpm_pin,
		GPIO_IRQ_EDGE_RISE,
		true,
		&tachometer_callback);
}

void fan::adjust(float temperature) {
	if (temperature <= _min_temp) {
		set_speed_percent(0);
		return;
	}

	if (temperature >= _max_temp) {
		set_speed_percent(100);
		return;
	}

	const float step = 100.0f / (_max_temp - _min_temp);
	set_speed_percent((temperature - _min_temp) * step);
}

void fan::set_speed_percent(uint8_t percent) {
	if (percent > 100) {
		percent = 100;
	}

	if (_speed_percent != percent) {
		pwm_set_chan_level(_pwm_slice, _pwm_channel, percent);
		_speed_percent = percent;
	}
}

uint32_t fan::rpm() {
	uint32_t now = time_us_32();
	uint32_t revolutions = _revolutions[_rpm_pin];
	uint32_t diff = now - _rpm_last_measure_time;
	_revolutions[_rpm_pin] = 0;
	_rpm_last_measure_time = now;
	// There are two pulses for each revolution, hence the 30s
	return (30000000 / diff) * revolutions;
}
