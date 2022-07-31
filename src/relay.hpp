#pragma once

#include <pico/stdlib.h>

class relay {
public:
	typedef bool(*toggle_condition_t)(void**);

	relay(
		uint pin,
		toggle_condition_t toggle_condition,
		void** toggle_data,
		int32_t check_inteval_ms);

	void toggle(bool value);

	const char* status() const;

private:
	static bool repeating_timer_callback(repeating_timer* timer);

	const uint _pin;
	const toggle_condition_t _toggle_condition;
	void** _toggle_data;
	const int32_t _check_inteval_ms;
	volatile bool _state;
	repeating_timer _timer;
};