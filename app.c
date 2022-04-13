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
#include "em_chip.h"
#include "sl_simple_timer.h"

#include "sl_sleeptimer.h"

/*---- Include User's Header ----*/
#include "i2c_lib.h"
#include "lm75.h"
#include "max30102.h"
#include "bpm_spo2_calc.h"
#include "gpio_intr.h"
#include "led_buzzer.h"
#include "msc.h"
#include "mpu6050.h"
#include "test_variable.h"
#include "time.h"
#include "em_rtcc.h"
#include "function.h"
uint8_t notifyEnabled = false;
uint8_t indicateEnabled = false;
uint8_t app_connection;

// The advertising set handle allocated from Bluetooth stack.
static uint8_t advertising_set_handle = 0xff;

/*----------------- Define Area -----------------*/
#define TIMER_MS(ms) ((32768 * ms) / 1000)
#define TEMPERATURE 0
/*MPU6050 */
volatile bool mpuInterrupt = false;     // indicates whether MPU interrupt pin has gone high
void dmpDataReady() {
  mpuInterrupt = true;
}
struct MPU6050_Base mpu;
uint16_t packetSize;
uint8_t devStatus;
uint8_t mpuIntStatus;
bool dmpReady = false;
struct tm time_date;
float te = 36.125;
float spo2 = 96.0;
float bmp = 80.0;

sl_sleeptimer_date_t datetest;
/**************************************************************************//**
 * Application Init.
 *****************************************************************************/
SL_WEAK void app_init (void)
{
  /////////////////////////////////////////////////////////////////////////////
  // Put your additional application init code here!                         //
  // This is called once during start-up.                                    //
  /////////////////////////////////////////////////////////////////////////////
  sl_app_log("Initiation.... \n");

  // Chip init
  CHIP_Init ();
  sl_app_log(" Chip init Ok \n");

  // I2C init
  i2c_init ();
  sl_app_log(" I2C init Ok \n");

  // Max30102 init
  MAX30102_init ();
  sl_app_log(" MAX30102 init Ok \n");

  // PPG init
  PPG_init ();
  sl_app_log(" PPG init Ok \n");

  // LED & Buzzer init
  led_buzzer_init ();
  sl_app_log(" LED & buzzer init Ok \n");

  // MSC init
  //msc_init ();
  sl_app_log(" MSC init Ok \n");

  // GPIO Interrupt init
  gpio_INTR_init();
  sl_app_log(" GPIO Intr init Ok \n");

  // MPU6050init
  //MPU6050_ConfigDMP(&mpu, &devStatus, &dmpReady, &mpuIntStatus, &packetSize);
  //	datetest.time_zone = 7;
  //	datetest.year = 122;
  //	datetest.month = 3;
  //	datetest.month_day = 8;
  //	datetest.hour = 15;
  //	datetest.min = 10;
  //	datetest.sec = 0;
  //	sl_sleeptimer_set_datetime(&datetest);
  ////	sl_sleeptimer_build_datetime(&datetest, 122, 3, 8, 15, 6, 0, 7);
  //	//RTCC init
  //
  //
  ////	  RTCC_Init_TypeDef rtcc = RTCC_INIT_DEFAULT;
  ////	  rtcc.presc = rtccCntPresc_32768;
  ////	  RTCC_Init(&rtcc);
  ////    RTCC_CounterSet(t);
  ////    sl_app_log("Ok....... \n");
  sl_bt_system_set_soft_timer(TIMER_MS(2000), TEMPERATURE, 0);
  ////    time_t rawtime = time(NULL);
  ////    struct tm * timeinfo;
  ////
  ////    time ( &rawtime );
  ////    timeinfo = localtime ( &rawtime );
  ////    app_log ( "Current local time and date: %s", asctime (timeinfo));
}

/**************************************************************************//**
 * Application Process Action.
 *****************************************************************************/
SL_WEAK void app_process_action (void)
{
  /////////////////////////////////////////////////////////////////////////////
  // Put your additional application code here!                              //
  // This is called infinitely.                                              //
  // Do not call blocking functions from here!                               //
  /////////////////////////////////////////////////////////////////////////////
  /*********************** Khanh's Process **********************************/
  //	PPG_update ();
  //	set_Buzzer();
  /*********************** Duong's Process **********************************/
  // MPU6050_GetData(&mpu, &dmpReady, &mpuInterrupt, &packetSize, &mpuIntStatus);


}


void process_server_user_write_request(sl_bt_msg_t *evt)
{
  sl_status_t sc;
  uint32_t connection =
      evt->data.evt_gatt_server_user_write_request.connection;
  uint32_t characteristic =
      evt->data.evt_gatt_server_user_write_request.characteristic;
  sc = sl_bt_gatt_server_send_user_write_response(connection,characteristic,0);

  if (characteristic == gattdb_device_ch)
    {

      uint8_t header = evt->data.evt_gatt_server_attribute_value.value.data[0];
      uint8_t len = evt->data.evt_gatt_server_attribute_value.value.len;
      app_log("header: %d - len: %d \n",header, len);

      if (header == 1 && len ==  7 ){ //set date time
	  datetest.time_zone = 7;
	  datetest.year = evt->data.evt_gatt_server_attribute_value.value.data[3];
	  datetest.month = evt->data.evt_gatt_server_attribute_value.value.data[2];
	  datetest.month_day = evt->data.evt_gatt_server_attribute_value.value.data[1];
	  datetest.hour = evt->data.evt_gatt_server_attribute_value.value.data[4];
	  datetest.min = evt->data.evt_gatt_server_attribute_value.value.data[5];
	  datetest.sec = evt->data.evt_gatt_server_attribute_value.value.data[6];
	  sl_sleeptimer_set_datetime(&datetest);
      }
      	  else if(header == 2 && len == 1 )
      	    {
      	      send_data(&notifyEnabled, &app_connection, &te, 1);
      	    }
      	  else if(header == 3 && len ==1 )
      	    {
      	      send_data(&notifyEnabled, &app_connection, &spo2, 2);
      	    }
      	  else if(header == 4 && len ==1 )
      	    {
      	      send_data(&notifyEnabled, &app_connection, &bmp, 3);
      	    }
      	  else if(header == 5 && len ==1)
      	    {
      	      send_all_data(&notifyEnabled, &app_connection, &te, &spo2, &bmp);
      	    }
    }
}
  /**************************************************************************//**
   * Bluetooth stack event handler.
   * This overrides the dummy weak implementation.
   *
   * @param[in] evt Event coming from the Bluetooth stack.
   *****************************************************************************/
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
	sl_app_log("connection opened \n");
	break;

	// -------------------------------
	// This event indicates that a connection was closed.
      case sl_bt_evt_connection_closed_id:
	// Restart advertising after client has disconnected.
	sc = sl_bt_advertiser_start(
	    advertising_set_handle,
	    sl_bt_advertiser_general_discoverable,
	    sl_bt_advertiser_connectable_scannable);
	sl_app_log("connection closed \n");
	app_assert_status(sc);
	break;

	///////////////////////////////////////////////////////////////////////////
	// Add additional event handlers here as your application requires!      //
	///////////////////////////////////////////////////////////////////////////

      case sl_bt_evt_gatt_server_characteristic_status_id :
	if(evt->data.evt_gatt_server_characteristic_status.characteristic == gattdb_data_ch)
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
	//    case sl_bt_evt_gatt_server_attribute_value_id:
	//      if (evt->data.evt_gatt_server_characteristic_status.characteristic
	//          == gattdb_device_ch)
	//        {
	//          uint8_t header = evt->data.evt_gatt_server_attribute_value.value.data[0];
	//          if (header == 1){ //set date time
	//                datetest.time_zone = 7;
	//                datetest.year = evt->data.evt_gatt_server_attribute_value.value.data[3];
	//                datetest.month = evt->data.evt_gatt_server_attribute_value.value.data[2];
	//                datetest.month_day = evt->data.evt_gatt_server_attribute_value.value.data[1];
	//                datetest.hour = evt->data.evt_gatt_server_attribute_value.value.data[4];
	//                datetest.min = evt->data.evt_gatt_server_attribute_value.value.data[5];
	//                datetest.sec = evt->data.evt_gatt_server_attribute_value.value.data[6];
	//                sl_sleeptimer_set_datetime(&datetest);
	//          }
	//
	//        }
	//      break;
      case sl_bt_evt_gatt_server_user_write_request_id:
	process_server_user_write_request(evt);


	break;
	          case sl_bt_evt_system_soft_timer_id:
	            if (evt->data.evt_system_soft_timer.handle == TEMPERATURE)
	              {
//	                app_log("%d\n",123);
//	                sl_sleeptimer_get_datetime(&datetest);
//	                app_log("date time %d/%d/%d  %d:%d:%d\n",
//	                        datetest.month_day,
//	                        datetest.month,
//	                        datetest.year+1900,
//	                        datetest.hour,
//	                        datetest.min,
//	                        datetest.sec
//	                        );
	        	send_check(&notifyEnabled, &app_connection);

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
