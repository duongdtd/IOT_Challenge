/***************************************************************************//**
 * @file
 * @brief Core application logic.
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/
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
uint8_t notifyEnabled = false;
uint8_t indicateEnabled = false;
uint8_t app_connection;
#define TEMPERATURE 0
#define TIMESET(MS) (MS*32768/1000)
// The advertising set handle allocated from Bluetooth stack.
static uint8_t advertising_set_handle = 0xff;
float T;
max30102_t max30102;
struct tm time_date;
 struct Quaternion_Base q;           // [w, x, y, z]         quaternion container
 struct VectorInt16_Base aa;         // [x, y, z]            accel sensor measurements
 struct VectorInt16_Base aaReal;     // [x, y, z]            gravity-free accel sensor measurements
 struct VectorInt16_Base aaWorld;    // [x, y, z]            world-frame accel sensor measurements
 struct VectorFloat_Base gravity;
 volatile bool mpuInterrupt = false;     // indicates whether MPU interrupt pin has gone high
 void dmpDataReady() {
     mpuInterrupt = true;
 }
float euler[3];         // [psi, theta, phi]    Euler angle container
float ypr[3];
uint8_t devStatus;
uint8_t mpuIntStatus;
bool dmpReady = false;
uint16_t packetSize;
uint16_t fifoCount;     // count of all bytes currently in FIFO
uint8_t fifoBuffer[64];
struct MPU6050_Base mpu;
double ax;
double ay;
double az;
double SVM;
/**************************************************************************//**
 * Application Init.
 *****************************************************************************/
SL_WEAK void app_init(void)
{
  /////////////////////////////////////////////////////////////////////////////
  // Put your additional application init code here!                         //
  // This is called once during start-up.                                    //
  /////////////////////////////////////////////////////////////////////////////
    CHIP_Init();
    gpio_INTR_Init();
    led_buzzer_init();
    mpu.dmpPacketSize = 42;
    mpu.fifoTimeout=MPU6050_FIFO_DEFAULT_TIMEOUT;
    led_buzzer_init();
   // setLED('r');
    i2c_init();
    MPU6050_ConfigDMP(&mpu, &devStatus, &dmpReady, &mpuIntStatus, &packetSize);
}


/**************************************************************************//**
 * Application Process Action.
 *****************************************************************************/
SL_WEAK void app_process_action (void)
{

  // if programming failed, don't try to do anything
  MPU6050_GetData(&mpu, &dmpReady, &mpuInterrupt, &packetSize, &mpuIntStatus);

}

/**************************************************************************//**
 * Bluetooth stack event handler.
 * This overrides the dummy weak implementation.
 *
 * @param[in] evt Event coming from the Bluetooth stack.
 *****************************************************************************/

void convert_data(uint8_t arr[],float *data)
{
  int16_t n;
  n=(*data)*1000;
  arr[0] = n/1000;
  arr[1] = (n-arr[0]*1000)/125;

}
void
send_notify (void)
{
  sl_status_t sc;
  uint8_t buffer[2];
  buffer[1] = 10;
  buffer[0] = 5;
  uint16_t len = 2;
  if (notifyEnabled)
    {
      sc = sl_bt_gatt_server_send_notification (app_connection, gattdb_temp_ch,
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
  sc = sl_bt_gatt_server_write_attribute_value(gattdb_temp_ch, 0, sizeof(buffer),
                                              buffer);
  if (sc == SL_STATUS_OK)
    {
      app_log("clear OK\n");
    }

}
void sl_bt_on_event(sl_bt_msg_t *evt)
{
  sl_status_t sc;
  bd_addr address;
  uint8_t address_type;
  uint8_t system_id[8];

  switch (SL_BT_MSG_ID(evt->header)) {
    // -------------------------------
    // This event indicates the device has started and the radio is ready.
    // Do not call any stack command before receiving this boot event!
    case sl_bt_evt_system_boot_id:

      // Extract unique ID from BT Address.
      sc = sl_bt_system_get_identity_address(&address, &address_type);
      app_assert_status(sc);

      // Pad and reverse unique ID to get System ID.
      system_id[0] = address.addr[5];
      system_id[1] = address.addr[4];
      system_id[2] = address.addr[3];
      system_id[3] = 0xFF;
      system_id[4] = 0xFE;
      system_id[5] = address.addr[2];
      system_id[6] = address.addr[1];
      system_id[7] = address.addr[0];

      sc = sl_bt_gatt_server_write_attribute_value(gattdb_system_id,
                                                   0,
                                                   sizeof(system_id),
                                                   system_id);
      app_assert_status(sc);

      // Create an advertising set.
      sc = sl_bt_advertiser_create_set(&advertising_set_handle);
      app_assert_status(sc);

      // Set advertising interval to 100ms.
      sc = sl_bt_advertiser_set_timing(
        advertising_set_handle,
        160, // min. adv. interval (milliseconds * 1.6)
        160, // max. adv. interval (milliseconds * 1.6)
        0,   // adv. duration
        0);  // max. num. adv. events
      app_assert_status(sc);
      // Start general advertising and enable connections.
      sc = sl_bt_advertiser_start(
        advertising_set_handle,
        sl_bt_advertiser_general_discoverable,
        sl_bt_advertiser_connectable_scannable);
      app_assert_status(sc);
      break;

    // -------------------------------
    // This event indicates that a new connection was opened.
    case sl_bt_evt_connection_opened_id:
      app_connection = evt->data.evt_connection_opened.connection;
      break;

    // -------------------------------
    // This event indicates that a connection was closed.
    case sl_bt_evt_connection_closed_id:
      // Restart advertising after client has disconnected.
      sc = sl_bt_advertiser_start(
        advertising_set_handle,
        sl_bt_advertiser_general_discoverable,
        sl_bt_advertiser_connectable_scannable);
      app_assert_status(sc);
      break;

    ///////////////////////////////////////////////////////////////////////////
    // Add additional event handlers here as your application requires!      //
    ///////////////////////////////////////////////////////////////////////////

    case sl_bt_evt_gatt_server_characteristic_status_id :
      if(evt->data.evt_gatt_server_characteristic_status.characteristic == gattdb_temp_ch)
        {
          if(evt->data.evt_gatt_server_characteristic_status.status_flags == 0x01)
            {
              notifyEnabled = true;
              app_log("enable notifyEnabled \n");
            }
          else if(evt->data.evt_gatt_server_characteristic_status.status_flags == 0x02)
            {
              indicateEnabled = true;
              app_log("enable indicateEnabled \n");
            }
          else if(evt->data.evt_gatt_server_characteristic_status.status_flags == 0x00)
            {
              notifyEnabled = false;
              indicateEnabled = false;
              app_log("disable \n");
            }
        }

      break;
    case sl_bt_evt_gatt_server_attribute_value_id:
      if (evt->data.evt_gatt_server_characteristic_status.characteristic
          == gattdb_temp_ch)
        {
          uint8_t data = evt->data.evt_gatt_server_attribute_value.value.data[0];
          app_log("data :%d\n", data);
          if (data == 1)
            {
              clear_data ();
            }
        }
      break;
    case sl_bt_evt_system_soft_timer_id:
      if (evt->data.evt_system_soft_timer.handle == TEMPERATURE)
        {
          send_notify ();
        }
      break;
    case sl_bt_evt_system_external_signal_id:
      dmpDataReady();
      break;
    // -------------------------------
    // Default event handler.
    default:
      break;
  }
}


