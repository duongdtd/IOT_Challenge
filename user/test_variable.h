/*
 * test_variable.h
 *
 *  Created on: Mar 28, 2022
 *      Author: ADMIN
 */

#ifndef USER_TEST_VARIABLE_H_
#define USER_TEST_VARIABLE_H_
#include "stdio.h"
#include "stdint.h"
struct Quaternion_Base{
        float w;
        float x;
        float y;
        float z;
};
 struct VectorInt16_Base {
        int16_t x;
        int16_t y;
        int16_t z;
};
 struct VectorFloat_Base {
       float x;
       float y;
       float z;
};
///quaternion ///
void setQuaternion_Init(struct Quaternion_Base * q);

/// vectorInt16 ///
void setVectorInt16_Init(struct VectorInt16_Base * v);

/// VectorFloat ///
void setVectorFloat_Init(struct VectorFloat_Base * v);




#endif /* USER_TEST_VARIABLE_H_ */
