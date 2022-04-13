/*
 * gpio_intr.h
 *
 *  Created on: 18 thg 3, 2022
 *      Author: Ngo Minh Khanh
 */


#ifndef USER_GPIO_INTR_H_
#define USER_GPIO_INTR_H_


#include "em_chip.h"
#include "em_gpio.h"
#include "em_emu.h"
#include "em_cmu.h"

#define  intr_port      gpioPortB
#define  intr_pin       1

#define  LED_on_board_port     gpioPortA
#define  LED_on_board_pin     4

void gpio_INTR_init (void);
//void GPIO_EVEN_IRQHandler(void);
//void GPIO_ODD_IRQHandler(void);
void IRQ_Handler (void);
void ad5940_gpio_ext_handler (uint32_t int_num);


#endif /* USER_GPIO_INTR_H_ */
