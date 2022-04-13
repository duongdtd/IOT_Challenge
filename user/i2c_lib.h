/*
 * i2c_lib.h
 *
 *  Created on: 18 thg 2, 2022
 *      Author: Ngo Minh Khanh
 */

#ifndef USER_I2C_LIB_H_
#define USER_I2C_LIB_H_

#include "stdint.h"
// I2C Setting
// <o> Reference clock frequency
// <i> Frequency in Hz of the reference clock.
// <i> Select 0 to use the frequency of the currently selected clock.
// <i> Default: 0
#define I2C_REFERENCE_CLOCK 0

// <o>  Speed mode
// <0=> Standard mode (100kbit/s)
// <1=> Fast mode (400kbit/s)
// <2=> Fast mode plus (1Mbit/s)
// <i> Default: 0
#define I2C_SPEED_MODE    1
// </h> end I2CSPM config

// <<< end of configuration section >>>

// <<< sl:start pin_tool >>>
// <i2c signal=SCL,SDA> SL_I2CSPM_SENSOR
// $[I2C_SL_I2CSPM_SENSOR]
#define I2C_PERIPHERAL         I2C0
#define I2C_PERIPHERAL_NO      0

// I2C0 SCL on PD02
#define I2C_SCL_PORT                gpioPortD
#define I2C_SCL_PIN                 2

// I2C0 SDA on PD03
#define I2C_SDA_PORT               gpioPortD
#define I2C_SDA_PIN                3
// [I2C_SL_I2CSPM_SENSOR]$
// <<< sl:end pin_tool >>>

#define I2C_TXBUFFER_SIZE                 10
#define I2C_RXBUFFER_SIZE                 10

void i2c_init (void);
void i2c_writeByte (uint16_t followerAddress, uint8_t targetAddress,
					uint8_t txBuff);
void i2c_write_nBytes (uint16_t deviceAddress, uint8_t regAddress,
					   uint8_t *txData, uint8_t numBytes);
void i2c_read (uint16_t followerAddress, uint8_t targetAddress, uint8_t *rxBuff,
			   uint8_t numBytes);
void i2c_writeBit (uint8_t followerAddress, uint8_t targetAddress,
				   uint8_t bit_n, uint8_t data);
void i2c_writeBits (uint8_t followerAddress, uint8_t targetAddress,
					uint8_t start_bit, uint8_t len, uint8_t data);
void i2c_readBits (uint8_t slave_address, uint8_t regAddr, uint8_t bitStart,
				   uint8_t length, uint8_t *data);

#endif /* USER_I2C_LIB_H_ */
