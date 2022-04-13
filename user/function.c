/*
 * funtion.c
 *
 *  Created on: Apr 8, 2022
 *      Author: ADMIN
 */
#include "em_common.h"
#include "app_assert.h"
#include "sl_bluetooth.h"
#include "gatt_db.h"
#include "app.h"
#include "sl_app_log.h"
#include "led_buzzer.h"
#include "em_chip.h"
#include "i2c_lib.h"
#include "lm75.h"
#include "max30102.h"
#include "mpu6050.h"
#include "em_rtcc.h"
#include "gpio_intr.h"
#include "time.h"
#include "test_variable.h"
#include "math.h"
void convert_data(uint8_t arr[],float *data)
{
  arr[0] = (uint8_t) (*data);
  arr[1] = ((*data)-arr[0])/0.125;

}

void
send_data(uint8_t *notifyEnabled,uint8_t *app_connection,float *data,uint8_t type)
{
  sl_status_t sc;
  uint8_t buffer[8];
  uint8_t arr[1];
  uint16_t len = 9;
  sl_sleeptimer_date_t date_time;
  sl_sleeptimer_get_datetime(&date_time);
  switch(type)
  {
    case 1:

      convert_data(arr, data);
      buffer[2] = arr[1];
      buffer[1] = arr[0];
      buffer[0] = 1;
      break;
    case 2:
      buffer[2] = 0;
      buffer[1] = (uint8_t) (*data);
      buffer[0] = 2;
      break;
    case 3:
      buffer[2] = 0;
      buffer[1] = (uint8_t) (*data);
      buffer[0] = 3;
      break;
  }

  buffer[3] =date_time.month_day;
  buffer[4] =date_time.month;
  buffer[5] =date_time.year;
  buffer[6] =date_time.hour;
  buffer[7] =date_time.min;
  buffer[8] =date_time.sec;
  if (*notifyEnabled)
    {
      sc = sl_bt_gatt_server_send_notification (*app_connection, gattdb_data_ch,
                                                len, buffer);
    }
  if (sc == SL_STATUS_OK)
    {
      app_log("send ok\n");
    }
  else
    app_log("send erorr\n");

}
void
clear_data (void)
{
  sl_status_t sc;
  uint8_t buffer[2];
  for(int i=0;i<2;i++)
    {
      buffer[i] = 0;
    }
  sc = sl_bt_gatt_server_write_attribute_value(gattdb_device_ch, 0, sizeof(buffer),
                                              buffer);
  if (sc == SL_STATUS_OK)
    {
      app_log("clear OK\n");
    }

}
