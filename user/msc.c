/*
 * msc.c
 *
 *  Created on: 6 thg 4, 2022
 *      Author: Admin
 */
#include "msc.h"
#include "em_cmu.h"
#include "sl_app_log.h"

uint32_t word1 = 0x01;
uint32_t word2 = 0x02;
uint16_t count = 0;
bool isFull = false;
uint8_t a = 0x01;

void msc_init (void)
{
	// Enable MSC Clock
	CMU_ClockEnable (cmuClock_MSC, true);
}

void msc_write (void)
{
	if (count == 256)
		sl_app_log(" Full \n");
	else
	{
		MSC_Init ();
		MSC_WriteWord ((USERDATA + count), &word1, 4);
		count += 1;
		MSC_WriteWord ((USERDATA + count), &word2, 4);
		count += 1;
		MSC_Deinit ();
		for (int i = 0; i < 2; i++)
			sl_app_log(" Word %d: 0x%x \n", count - 2 + i, USERDATA[count-2+i]);
	}
}
