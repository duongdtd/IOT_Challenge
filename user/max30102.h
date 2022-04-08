/*
 * max30102.h
 *
 *  Created on: 23 thg 2, 2022
 *      Author: Ngo Minh Khanh
 */

#include "stdint.h"

#define MAX30102_ADDRESS    0x57

#define REG_INTR_STATUS_1   0x00
#define REG_INTR_STATUS_2   0x01
#define REG_INTR_ENABLE_1   0x02
#define REG_INTR_ENABLE_2   0x03
#define REG_FIFO_WR_PTR     0x04
#define REG_OVF_COUNTER     0x05
#define REG_FIFO_RD_PTR     0x06
#define REG_FIFO_DATA       0x07
#define REG_FIFO_CONFIG     0x08
#define REG_MODE_CONFIG     0x09
#define REG_SPO2_CONFIG     0x0A
#define REG_LED1_PA         0x0C
#define REG_LED2_PA         0x0D
#define REG_MULTI_LED_CTRL1 0x11
#define REG_MULTI_LED_CTRL2 0x12
#define REG_TEMP_INTR       0x1F
#define REG_TEMP_FRAC       0x20
#define REG_TEMP_CONFIG     0x21
#define REG_PROX_INT_THRESH 0x30
#define REG_REV_ID          0xFE
#define REG_PART_ID         0xFF

// 0x00~0xFF = 0mA~51mA
#define MAX_CURRENT  0xFF
#define FIX_CURRENT  0x82  // ~26mA for LED1 & LED2

#define  MAX30102_RESET           0x04
#define  MAX30102_MODE_HR_ONLY    0b02
#define  MAX30102_MODE_SPO2_HR    0x03
#define  MAX30102_MODE_MULTI_LED  0x07

// FIFO Sample Averaging
#define smp1   (0b000 << 5)
#define smp2   (0b001 << 5)
#define smp4   (0b010 << 5)
#define smp8   (0b011 << 5)
#define smp16  (0b100 << 5)
#define smp32  (0b101 << 5)

// SpO2 ADC Range Control
#define  adc2048   (0b00 << 5)
#define  adc4096   (0b01 << 5)
#define  adc8192   (0b10 << 5)
#define  adc16384  (0b11 << 5)

// SpO2 Sample Rate
#define   sr50    (0b000 << 2) // 50 samples per second
#define   sr100   (0b001 << 2) // 100 samples per second
#define   sr200   (0b010 << 2) // 200 samples per second
#define   sr400   (0b011 << 2) // 400 samples per second
#define   sr800   (0b100 << 2) // 800 samples per second
#define   sr1000  (0b101 << 2) // 1000 samples per second
#define   sr1600  (0b110 << 2) // 1600 samples per second
#define   sr3200  (0b111 << 2) // 3200 samples per second

// LED Pulse Width
// This is the same for both LEDs
#define  pw69   0x00    // 69us pulse
#define  pw118  0b01    // 118us pulse
#define  pw215  0b10    // 215us pulse
#define  pw411  0b11  // 411us pulse

#define FIFO_ROLLOVER_EN   (0b1 << 4)
#define FIFO_ROLLOVER_DIS  (0b0 << 4)

#define I2C_BUFFER_LENGTH 32

typedef struct max30102_t
{
    uint8_t BPM;
    uint8_t SpO2;
} max30102_t;

extern uint32_t raw_IR;
extern uint32_t raw_RED;

void MAX30102_init();
void MAX30102_ReadFIFO();
void MAX30102_Read();
void MAX30102_Shutdown(bool mode);

