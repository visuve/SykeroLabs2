#pragma once

#include <pico.h>

namespace pins {
	enum : uint8_t {
		ONBOARD_TEMPERATURE_IN = PICO_DEFAULT_I2C_SDA_PIN,

		RELAY1_OUT = 6,
		RELAY2_OUT = 7,

		FAN1_RPM_IN = 8,
		FAN1_PWM_OUT = 9,

		FAN2_RPM_IN = 10,
		FAN2_PWM_OUT = 11,

		RTC_I2C_SDA = 20,
		RTC_I2C_SCL = 21,

		DS18B20_IN = 22,

		ONBOARD_LED = PICO_DEFAULT_LED_PIN,
		LAST
	};
}