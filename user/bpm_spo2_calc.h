/*
 * BPM_spo2_calc.h
 *
 *  Created on: 6 thg 3, 2022
 *      Author: Pham Minh Hanh
 */
#include "stdint.h"

#ifndef USER_BPM_SPO2_CALC_H_
#define USER_BPM_SPO2_CALC_H_

typedef struct PPG
{
	uint32_t BPM;
	uint32_t AC;
	uint32_t DC;
} PPG;

typedef struct data_t
{
	double ir[1500];
	double red[1500];
} data_t;

extern PPG ppg_ir;
extern PPG ppg_red;

void PPG_init();
void swap(double* x, double* y);
double max(double *array, int array_size);
double sum(double *array, int array_size);
void sort(double* array, int array_size);
void DC_removal (uint32_t *raw_data, double *data, int n_sample, float alpha);
void median_filter(double* signal, int n_sample, int filter_size);
void BPM_estimator(double* signal, PPG* ppg, int n_sample, double thresh, float sample_rate);
double SpO2_estimator(double R);
void BPM_SpO2_estimator ();
void PPG_update ();

#endif /* USER_BPM_SPO2_CALC_H_ */
