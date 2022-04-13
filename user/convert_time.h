/*
 * convert_time.h
 *
 *  Created on: 9 thg 4, 2022
 *      Author: Admin
 */

#ifndef USER_CONVERT_TIME_H_
#define USER_CONVERT_TIME_H_

#include "time.h"
time_t convert_time_to_sec( struct tm *time);
struct tm convert_sec_to_time(time_t *sec);

#endif /* USER_CONVERT_TIME_H_ */
