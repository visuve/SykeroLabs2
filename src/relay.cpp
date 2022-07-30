#include "relay.hpp"

#include <hardware/gpio.h>

relay::relay(uint pin) :
	_pin(pin),
	_state(false) {
	gpio_init(pin);
	gpio_set_dir(pin, GPIO_OUT);
}

void relay::toggle(bool value) {
	if (_state != value) {
		gpio_put(_pin, value);
		_state = value;
	}
}