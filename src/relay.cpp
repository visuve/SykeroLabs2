#include "pch.hpp"
#include "relay.hpp"

relay::relay(
	uint pin,
	toggle_condition_t toggle_condition,
	void** toggle_data,
	int32_t check_inteval_ms) :
	_pin(pin),
	_toggle_condition(toggle_condition),
	_toggle_data(toggle_data),
	_check_inteval_ms(check_inteval_ms),
	_state(false) {
	gpio_init(pin);
	gpio_set_dir(pin, GPIO_OUT);

	if (toggle_condition) {

		toggle(_toggle_condition(_toggle_data));

		add_repeating_timer_ms(check_inteval_ms, repeating_timer_callback, this, &_timer);
	}
}

void relay::toggle(bool value) {
	if (_state != value) {
		gpio_put(_pin, value);
		_state = value;
	}
}

const char* relay::status() const {
	return _state ? "on" : "off";
}

bool relay::repeating_timer_callback(repeating_timer* timer) {
	if (!timer || !timer->user_data) {
		return false;
	}

	auto self = reinterpret_cast<relay*>(timer->user_data);
	bool result = self->_toggle_condition(self->_toggle_data);
	self->toggle(result);
	return true;
}