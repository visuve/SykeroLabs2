#pragma once

#include <pico/mutex.h>
#include <hardware/gpio.h>
#include "sykerolabs.hpp"

class mutex_guard {
public:
	inline mutex_guard(mutex_t* mutex) :
		_mutex(mutex) {
		mutex_enter_blocking(_mutex);
		gpio_put(pins::ONBOARD_LED, true);
	}

	inline ~mutex_guard() {
		gpio_put(pins::ONBOARD_LED, false);
		mutex_exit(_mutex);
	}

private:
	mutex_t* _mutex;
};

class recursive_mutex_guard {
public:
	inline recursive_mutex_guard(recursive_mutex_t* mutex) :
		_mutex(mutex) {
		recursive_mutex_enter_blocking(_mutex);
		gpio_put(pins::ONBOARD_LED, true);
	}

	inline ~recursive_mutex_guard() {
		gpio_put(pins::ONBOARD_LED, false);
		recursive_mutex_exit(_mutex);
	}

private:
	recursive_mutex_t* _mutex;
};