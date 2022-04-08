/*
 * hr_spo2_calc.h
 *
 *  Created on: 6 thg 3, 2022
 *      Author: Ngo Minh Khanh
 */
#include "stdint.h"

#ifndef USER_HR_SPO2_CALC_H_
#define USER_HR_SPO2_CALC_H_

#define MEAN_FILTER_SIZE        15
typedef struct{
   int32_t value[2];
}Filter_Data;

#define MAGIC_ACCEPTABLE_INTENSITY_DIFF         65000
#define RED_LED_CURRENT_ADJUSTMENT_NS           1000000

/* Pulse detection parameters */
#define PULSE_MIN_THRESHOLD         100 //300 is good for finger, but for wrist you need like 20, and there is shitloads of noise
#define PULSE_MAX_THRESHOLD         2000
#define PULSE_GO_DOWN_THRESHOLD     1
#define PULSE_BPM_SAMPLE_SIZE       10 //Moving average size
/* SaO2 parameters */
#define RESET_SPO2_EVERY_N_PULSES     100
typedef enum PulseStateMachine{
   PULSE_IDLE,
   PULSE_TRACE_UP,
   PULSE_TRACE_DOWN
}PulseStateMachine;

extern int32_t IRcw;
extern int32_t REDcw;

extern uint8_t redLEDCurrent;

int32_t dcRemove(int32_t value,int32_t *cw);
int32_t MeanDiff(int32_t M);
int32_t LowPassButterworthFilter(int32_t value,Filter_Data *filter_data);
//bool detectPulse(uint32_t sensor_value);

#endif /* USER_HR_SPO2_CALC_H_ */
