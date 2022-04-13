/*
 * msc.h
 *
 *  Created on: 6 thg 4, 2022
 *      Author: Ngo Minh Khanh
 */

#ifndef USER_MSC_H_
#define USER_MSC_H_

#include "em_msc.h"
#include <inttypes.h>

#define USERDATA ((uint32_t*)USERDATA_BASE)

void msc_init(void);
void msc_write(void);

#endif /* USER_MSC_H_ */
