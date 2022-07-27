#include "sykerolabs.hpp"
#include "onboard_temperature_sensor.hpp"

#include <hardware/adc.h>

// The ADC is 12 bit and the reference voltage is 3.3
constexpr float conversion_factor = 3.3f / (1 << 12);

onboard_temperature_sensor::onboard_temperature_sensor() {
	adc_init();
	adc_set_temp_sensor_enabled(true);
	adc_select_input(pins::ONBOARD_TEMPERATURE_IN);
}

float onboard_temperature_sensor::celcius() const {
	float voltage = adc_read() * conversion_factor;
	// This magic formula is from the RP2040 datasheet
	return 27.0f - (voltage - 0.706) / 0.001721;
}
