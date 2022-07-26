#include <pico.h>

namespace pins {
	enum : uint8_t {
		RELAY1_OUT = 6,
		RELAY2_OUT = 7,

		FAN1_PWM_OUT = 10,
		FAN1_RPM_IN = 11,

		FAN2_PWM_OUT = 12,
		FAN2_RPM_IN = 13,

		RTC_I2C_SCL = 20,
		RTC_I2C_SDA = 21,

		ONBOARD_LED = PICO_DEFAULT_LED_PIN,
		LAST
	};
}