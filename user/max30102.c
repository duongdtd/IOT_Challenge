/*
 * max30102.c
 *
 *  Created on: 23 thg 2, 2022
 *      Author: Ngo Minh Khanh
 */

#include <i2c_lib.h>
#include <max30102.h>
#include "sl_app_log.h"

uint16_t data = 0;
uint32_t raw_IR = 0;
uint32_t raw_RED = 0;

void MAX30102_init()
{
    // Reset
    i2c_write(MAX30102_ADDRESS, REG_MODE_CONFIG, MAX30102_RESET);

    // Mode Configuration
    i2c_write(MAX30102_ADDRESS, REG_MODE_CONFIG, MAX30102_MODE_SPO2_HR);

    //FIFO Configuration
    i2c_write(MAX30102_ADDRESS, REG_FIFO_CONFIG, smp2);

    // LED Pulse Amplitude Configuration
    i2c_write(MAX30102_ADDRESS, REG_LED1_PA, FIX_CURRENT);
    i2c_write(MAX30102_ADDRESS, REG_LED2_PA, FIX_CURRENT);

    // SpO2 Configuration
    i2c_write(MAX30102_ADDRESS, REG_SPO2_CONFIG, adc16384 | sr100 | pw411);

    // Clear FIFO
    i2c_write(MAX30102_ADDRESS, REG_FIFO_WR_PTR, 0x00);
    i2c_write(MAX30102_ADDRESS, REG_OVF_COUNTER, 0x00);
    i2c_write(MAX30102_ADDRESS, REG_FIFO_RD_PTR, 0x00);
    sl_app_log("IR RED\n");
}

void MAX30102_ReadFIFO()
{
    uint8_t writePointer = 0, readPointer = 0;
    i2c_read(MAX30102_ADDRESS,REG_FIFO_WR_PTR, &writePointer, 1);
    i2c_read(MAX30102_ADDRESS,REG_FIFO_RD_PTR, &readPointer, 1);

//    if(writePointer != readPointer)
//       sl_app_log("OK \n");

    int16_t num_samples;
    num_samples = (int16_t)writePointer - (int16_t)readPointer;

//    sl_app_log("%d %d %d\n",writePointer, readPointer, num_samples);

    if (num_samples < 1)
    {
        num_samples += 32;
    }

    uint8_t bytesLeftToRead = num_samples * 2 * 3;
    while (bytesLeftToRead > 0)
    {
        uint8_t toGet =  bytesLeftToRead;
        if (toGet > I2C_BUFFER_LENGTH)
        {
             toGet = I2C_BUFFER_LENGTH - (I2C_BUFFER_LENGTH % (2 * 3));
        }
        bytesLeftToRead -= toGet;
        while(toGet > 0)
        {
            uint8_t sample[6];
            i2c_read(MAX30102_ADDRESS, REG_FIFO_DATA, sample, 6);
            raw_RED = ((uint32_t)(sample[0] << 16) | (uint32_t)(sample[1] << 8) | (uint32_t)(sample[2])) & 0x3ffff;
            raw_IR = ((uint32_t)(sample[3] << 16) | (uint32_t)(sample[4] << 8) | (uint32_t)(sample[5])) & 0x3ffff;
            sl_app_log("%d, %d \n", raw_IR, raw_RED);
//            sl_app_log("%d\n", raw_IR);
            toGet -= 2 * 3;
         }
     }

//    for (int8_t i = 0; i < num_samples; i++)
//    {
//        uint8_t sample[6];
//        i2c_read(MAX30102_ADDRESS, REG_FIFO_DATA, sample, 6);
//        raw_RED = ((uint32_t)(sample[0] << 16) | (uint32_t)(sample[1] << 8) | (uint32_t)(sample[2])) & 0x3ffff;
//        raw_IR = ((uint32_t)(sample[3] << 16) | (uint32_t)(sample[4] << 8) | (uint32_t)(sample[5])) & 0x3ffff;
//        sl_app_log("%d\n", raw_IR);
//    }
}

void MAX30102_Shutdown(bool mode)
{
   uint8_t config;
   i2c_read(MAX30102_ADDRESS, REG_MODE_CONFIG, &config, 1);
   if(mode)
   {
       config = config | 0x80;
   }
   else {
     config = config | 0x7F;
   }
   i2c_write(MAX30102_ADDRESS,REG_MODE_CONFIG, config);
}

//void MAX30102_Read()
//{
//   MAX30102_ReadFIFO();
//   int32_t IR_ac = dcRemove(raw_IR,&IRcw);
//   IR_ac = MeanDiff(IR_ac);
//   IR_ac = LowPassButterworthFilter(IR_ac,&irf);
//   sl_app_log("%u\n", IR_ac);
////   detectPulse(IR_ac);
//}


