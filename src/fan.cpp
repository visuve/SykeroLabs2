#include <pico/stdlib.h>
#include <hardware/pwm.h>
#include <hardware/gpio.h>

#include "fan.hpp"

volatile uint32_t fan::_revolutions[] = {};

void fan::tachometer_callback(uint gpio, uint32_t) {
	++_revolutions[gpio];
}

fan::fan(uint pwm_pin, uint rpm_pin) :
	_pwm_slice(pwm_gpio_to_slice_num(pwm_pin)),
	_pwm_channel(pwm_gpio_to_channel(pwm_pin)),
	_rpm_pin(rpm_pin)
{
	gpio_set_function(pwm_pin, GPIO_FUNC_PWM);
	gpio_set_dir(rpm_pin, GPIO_IN);

	pwm_set_wrap(_pwm_slice, 100);
	pwm_set_clkdiv(_pwm_slice, 255);

	set_speed_percent(0);
	pwm_set_enabled(_pwm_slice, true);

	_revolutions[rpm_pin] = 0;

	gpio_set_irq_enabled_with_callback(
		rpm_pin,
		GPIO_IRQ_EDGE_RISE,
		true,
		&tachometer_callback);
}

void fan::set_speed_percent(uint16_t percent) {
	if (percent > 100) {
		percent = 100;
	}

	pwm_set_chan_level(_pwm_slice, _pwm_channel, percent);
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
