#pragma once

#include <pico/stdlib.h>

class relay {
public:
	relay(uint pin);
	void toggle(bool value);

private:
	uint _pin;
	bool _state;
};