#include <stdio.h>
#include <pico/stdlib.h>
#include <pico/util/queue.h>
#include <hardware/gpio.h>
#include <hardware/pwm.h>

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

class fan {
public:
	static void tachometer_callback(uint gpio, uint32_t /*events*/) {
		++_revolutions[gpio];
	}

	fan(uint pwm_pin, uint rpm_pin) :
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

	void set_speed_percent(uint16_t percent) {
		if (percent > 100) {
			percent = 100;
		}

		pwm_set_chan_level(_pwm_slice, _pwm_channel, percent);
	}

	uint32_t rpm() {
		uint32_t now = time_us_32();
		uint32_t revolutions = _revolutions[_rpm_pin];
		uint32_t diff = now - _rpm_last_measure_time;
		_revolutions[_rpm_pin] = 0;
		_rpm_last_measure_time = now;
		// There are two pulses for each revolution, hence the 30s
		return (30000000 / diff) * revolutions;
	}

private:
	uint _pwm_slice = 0;
	uint _pwm_channel = 0;

	uint _rpm_pin = 0;
	static volatile uint32_t _revolutions[pins::LAST];
	uint32_t _rpm_last_measure_time = 0;
};

volatile uint32_t fan::_revolutions[] = {};

int main() {
	stdio_init_all();

	gpio_init(pins::ONBOARD_LED);
	gpio_set_dir(pins::ONBOARD_LED, GPIO_OUT);

	fan fan1(pins::FAN1_PWM_OUT, pins::FAN1_RPM_IN);
	fan fan2(pins::FAN2_PWM_OUT, pins::FAN2_RPM_IN);

	puts("Fan 1 RPM; Fan 2 RPM");

	while (true) {
		gpio_put(pins::ONBOARD_LED, 1);
		sleep_ms(1000);
		gpio_put(pins::ONBOARD_LED, 0);
		sleep_ms(1000);

		printf("%u;%u\n", fan1.rpm(), fan2.rpm());
	}

	return 0;
}