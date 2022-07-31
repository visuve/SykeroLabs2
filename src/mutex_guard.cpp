#include "pch.hpp"
#include "mutex_guard.hpp"
#include "sykerolabs.hpp"

mutex_guard::mutex_guard(mutex_t* mutex) :
	_mutex(mutex) {
	mutex_enter_blocking(_mutex);
	gpio_put(pins::ONBOARD_LED, true);
}

mutex_guard::~mutex_guard() {
	gpio_put(pins::ONBOARD_LED, false);
	mutex_exit(_mutex);
}

recursive_mutex_guard::recursive_mutex_guard(recursive_mutex_t* mutex) :
	_mutex(mutex) {
	recursive_mutex_enter_blocking(_mutex);
	gpio_put(pins::ONBOARD_LED, true);
}

recursive_mutex_guard::~recursive_mutex_guard() {
	gpio_put(pins::ONBOARD_LED, false);
	recursive_mutex_exit(_mutex);
}