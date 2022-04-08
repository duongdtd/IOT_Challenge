/*
 * lm75.c
 *
 *  Created on: 18 thg 2, 2022
 *      Author: Ngo Minh Khanh
 */

#include <i2c_lib.h>
#include <lm75.h>
#include "app.h"
//
/**
* @brief      Read config register of LM75
*
* @return     Value of LM75 config register
*/
uint8_t LM75_ReadConfig(void){
  uint8_t config;
  i2c_read(LM75_ADDRESS,LM75_CONFIGURATION,&config,1);
  return config;
}

/**
 * @brief      Read 9-bit format temperature from register
 *
 * @param[in]  reg   -   address of temperature register
 * @return     Value of temperature
*/
float LM75_ReadTemperature_9BitReg()
{
  uint16_t value;
  i2c_read(LM75_ADDRESS,LM75_TEMPERATURE,(uint8_t*)&value,2);

  value = ((( value >> 8 ) | ( value << 8 )) >> 7) & 0x01FF;

  if( value & 0x0100 ){                   // kiem tra bit dau
    value = (0x01FE ^ value) + 1;         // đoi sang bu 2
    return (float)( value * (-0.5f) );
  }
  else{
    return (float)( value * 0.5f );
  }

}

/**
 * @brief      Read 11-bit format temperature from register
 *
 * @param[in]  reg   -   address of temperature register
 * @return     Value of temperature
*/
float LM75_ReadTemperature_11BitReg(void)
{
  uint16_t value;
  i2c_read(LM75_ADDRESS,LM75_TEMPERATURE,(uint8_t*)&value,2);

  value = ((( value >> 8 ) | ( value << 8 )) >> 5 ) & 0x07FF;

  if(value & 0x0400){                       // kiem tra bit dau
    value = (0x07FF ^ value) + 1;           // doi sang bu 2
    return  (float)(value * ( -0.125f ) );
  }
  else{
    return  (float)(value * 0.125f );
  }
}


/**
 * @brief      Config LM75 into sleep mode (shutdown)
 *
 * @param[in]  mode    - 0: normal (đefaul); 1: shutdown
 * @return     none
*/
void LM75_Shutdown(uint8_t mode)
{
  uint8_t buff;
  buff = LM75_ReadConfig();

  if( mode ){
    buff = buff | 0x01;
  }
  else{
    buff = buff & 0xFE;
  }
  i2c_write(LM75_ADDRESS,LM75_CONFIGURATION,buff);
}


/**
 * @brief    Read temperature
 *
 * @return   value of temperature
*/

float LM75_ReadTemperature(void){

  #ifdef LM75_11BIT
    return LM75_ReadTemperature_11BitReg();

  #endif

  #ifdef LM75_9BIT
    return LM75_ReadTemperature_9BitReg();
  #endif
}
//
//
