/*
 * led_buzzer.h
 *
 *  Created on: 22 thg 2, 2022
 *      Author: Ngo Minh Khanh
 */

#include "em_gpio.h"
#include "sl_app_log.h"

#define  red_LED_port     gpioPortC
#define  red_LED_pin      1

#define  green_LED_port   gpioPortB
#define  green_LED_pin    4

#define  blue_LED_port    gpioPortB
#define  blue_LED_pin     3

#define  buzzer_port      gpioPortC
#define  buzzer_pin       0

#define  button_port      gpioPortC
#define  button_pin       7

#define  LED_on_board_port     gpioPortA
#define  LED_on_board_pin     4

void led_buzzer_init(void);
void setBuzzer(bool set);
void setLED(char ch);
void blynk(void);

