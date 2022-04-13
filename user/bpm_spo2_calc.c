/*
 * bpm_spo2_calc.c
 *
 *  Created on: 6 thg 3, 2022
 *      Author: Pham Minh Hanh
 */

#include "bpm_spo2_calc.h"
#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>
#include "sl_app_log.h"
#include "max30102.h"

fifo_t fifo_data;

data_t data;
int n_ir = 1200;
int n_red = 1200;
double ir[1200];
double red[1200];

void PPG_init()
{
	fifo_data.cnt = 0;
}

void PPG_update ()
{

	MAX30102_ReadFIFO (&fifo_data);
	if(fifo_data.cnt == STORAGE_SIZE)
	{
//		BPM_SpO2_estimator();
		fifo_data.cnt = 0;
	}
}

void BPM_SpO2_estimator()
{
	// DC removal
	float alpha = 0.95;
	DC_removal (fifo_data.raw_IR, ir, n_ir, alpha);
	DC_removal (fifo_data.raw_RED, red, n_red, alpha);

	// Get DC components of ir signal and red signal
	double DC_ir = max (ir, n_ir);
	double DC_red = max (red, n_red);
	ppg_ir.DC = DC_ir;
	ppg_red.DC = DC_red;

	// applying median filter
	int filter_size = 5;
	median_filter (ir, n_ir, filter_size);
	median_filter (red, n_red, filter_size);

	// Estimate BPM
	double thresh = 0.0;
	float sample_rate = 100.0;
	BPM_estimator (ir, &ppg_ir, n_ir, thresh, sample_rate);
	BPM_estimator (red, &ppg_red, n_red, thresh, sample_rate);

	// Calculate Spo2
	double R = (ppg_ir.AC / DC_ir) / (ppg_red.AC / DC_red);
	double spo2 = SpO2_estimator (R);

	sl_app_log(" BPM: %d - SpO2: %d \n", ppg_ir.BPM, spo2);
}

void DC_removal (uint32_t *raw_data, double *data, int n_sample, float alpha)
{
	double pre_w = 0.0;
//	int now_w = 0;
	int i;
	for (i = 0; i < n_sample; i++)
	{
		double now_w = raw_data[i] + (alpha * pre_w);
		data[i] = now_w - pre_w;
		pre_w = now_w;
	}
}

void median_filter(double* signal, int n_sample, int filter_size)
{
    int indexer = filter_size / 2;
    int window[filter_size];
    double temp[filter_size];
    int i;
    for(i = 0; i < filter_size; i ++){
        window[i] = i - indexer;
    }

    for(i =0; i < n_sample; i ++){
        int j;
        for(j = 0; j < filter_size; j ++){
            if(i + window[j] < 0 || n_sample <= i + window[j]){
                temp[j] = 0;
            } else{
                temp[j] = signal[i + window[j]];
            }
        }
        sort(temp, filter_size);
        signal[i] = temp[indexer];
    }
}

void BPM_estimator(double* signal, PPG* ppg, int n_sample, double thresh, float sample_rate)
{
    int count_p = 0;
    int cloc = 0;
    int ploc = 0;
    int peaks = 0;

    double interval = 0.0;
    int interval_count = 0 ;

    double lmax = 0;
    float nmax = 0;


    bool trace_up = false;
    bool trace_down = false;
    int i ;

    for(i = 1; i < n_sample; i++){
        if(signal[i] >= thresh && signal[i-1] < thresh){
            trace_up = true;
        }else{
            trace_up = false;
        }

        if(signal[i] <= thresh && signal[i-1] > thresh){
            trace_down = true;
        }else{
            trace_down = false;
        }

        if(trace_up == true || trace_down == true){
            count_p ++;
            if (peaks == 0) {
                peaks++;
            }
            if (count_p % 2 == 0) {
                cloc = i;
                int c_interval = cloc - ploc;
                interval = interval + c_interval;
                interval_count ++;

                if (c_interval > (interval / interval_count / 2)){
                    peaks++;
                }

                if (ploc != 0){
                    double max_lreg = max(&signal[ploc], cloc - ploc);
                    lmax = lmax + max_lreg;
                    nmax ++;
                }
                ploc = cloc;
            }
        }

    }

    double bpm = peaks / (n_sample / sample_rate) * 60.0;
    double AC = lmax / nmax;

    ppg->BPM = bpm;
    ppg->AC = AC;
}

double SpO2_estimator(double R)
{
    double spo2 = 0.0;
    if(0.4 <= R && R <= 1){
        spo2 = 110.0 - 25.0 * R;
    } else if(R <= 2.0){
        spo2 = 120.0 - 35.0 * R;
    } else if(R <= 3.5){
        spo2 = 350.0 / 3.0 - 100.0 / 3.0 * R;
    }
    if(spo2 > 100.0){
        spo2 = 100.0;
    } else if (spo2 < 80.0){
        spo2 = 80.0;
    }
    return spo2;
}

void sort(double* array, int array_size)
{
    int i, j;
    int min_idx;
    for (i = 0; i < (array_size) - 1; i++) {
        min_idx = i;
        for (j = i + 1; j < (array_size); j++)
            if (array[j] < array[min_idx])
                min_idx = j;
        swap(&array[min_idx], &array[i]);
    }
}

void swap(double* x, double* y)
{
    double temp = *x;
    *x = *y;
    *y = temp;
}

double max(double *array, int array_size)
{
	int i;
	double m = array[0];
	for(i = 1; i < array_size; i++){
		if(array[i] > m)
			m = array[i];
	}
	return m;
}

double sum(double *array, int array_size)
{
    double s = 0;
    int i ;
    for( i = 0; i < array_size; i++){
        s = s + array[i];
    }
    return s;
}

