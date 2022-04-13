/*
 * led_buzzer.c
 *
 *  Created on: 22 thg 2, 2022
 *      Author: Ngo Minh Khanh
 */

#include "led_buzzer.h"
#include "em_gpio.h"
#include "em_cmu.h"
#include "sl_pwm.h"
#include "sl_pwm_instances.h"

/**
 *  @brief init clock & set mode for led & buzzer
 */
void led_buzzer_init (void)
{
	CMU_ClockEnable (cmuClock_GPIO, true);
	GPIO_PinModeSet (red_LED_port, red_LED_pin, gpioModePushPull, 0);     // red
	GPIO_PinModeSet (green_LED_port, green_LED_pin, gpioModePushPull, 0); // green
	GPIO_PinModeSet (blue_LED_port, blue_LED_pin, gpioModePushPull, 0);  // blue
	GPIO_PinModeSet (buzzer_port, buzzer_pin, gpioModePushPull, 1);    // buzzer
	GPIO_PinModeSet (button_port, button_pin, gpioModeInput, 1);       // button
	GPIO_PinModeSet (LED_on_board_port, LED_on_board_pin, gpioModePushPull, 1);
	GPIO_PinOutSet (LED_on_board_port, LED_on_board_pin);
}

/**
 *  @brief set buzzer
 */
void set_Buzzer ()
{
	GPIO_PinOutSet (buzzer_port, buzzer_pin);
	sl_pwm_start (&sl_pwm_buzzer);
}

/**
 *  @brief clear buzzer
 */
void clear_Buzzer ()
{
	GPIO_PinOutClear (buzzer_port, buzzer_pin);
	sl_pwm_stop (&sl_pwm_buzzer);
}

/**
 *  @brief set color of led
 */
void set_LED (char ch)
{
	switch (ch)
	{
		case 'r':    // red
			GPIO_PinOutSet (red_LED_port, red_LED_pin);
			GPIO_PinOutClear (green_LED_port, green_LED_pin);
			GPIO_PinOutClear (blue_LED_port, blue_LED_pin);
			break;
		case 'g':    // green
			GPIO_PinOutClear (red_LED_port, red_LED_pin);
			GPIO_PinOutSet (green_LED_port, green_LED_pin);
			GPIO_PinOutClear (blue_LED_port, blue_LED_pin);
			break;
		case 'b':    // blue
			GPIO_PinOutClear (red_LED_port, red_LED_pin);
			GPIO_PinOutClear (green_LED_port, green_LED_pin);
			GPIO_PinOutSet (blue_LED_port, blue_LED_pin);
			break;
		case 'w':    // white
			GPIO_PinOutSet (red_LED_port, red_LED_pin);
			GPIO_PinOutSet (green_LED_port, green_LED_pin);
			GPIO_PinOutSet (blue_LED_port, blue_LED_pin);
			break;
	}
}

/**
 *  @brief clear all led
 */
void clear_all_LED ()
{
	GPIO_PinOutClear (red_LED_port, red_LED_pin);
	GPIO_PinOutClear (green_LED_port, green_LED_pin);
	GPIO_PinOutClear (blue_LED_port, blue_LED_pin);
}

void blynk (void)
{
	if (!GPIO_PinInGet (button_port, button_pin))
	{
		GPIO_PinOutSet (LED_on_board_port, LED_on_board_pin);
		sl_app_log("Led on \n");
	}
	else
	{
		GPIO_PinOutClear (LED_on_board_port, LED_on_board_pin);
		sl_app_log("Led off \n");
	}
}

