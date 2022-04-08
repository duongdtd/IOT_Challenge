/*
 * mpu6050.h
 *
 *  Created on: Mar 30, 2022
 *      Author: ADMIN
 */

#ifndef USER_MPU6050_H_
#define USER_MPU6050_H_
/*
 * mpu6050.h
 *
 *  Created on: 11 thg 3, 2022
 *      Author: Admin
 */
#include "stdbool.h"
#include "stdio.h"
#include "stdint.h"
#include "test_variable.h"
#define strcpy_P(dest, src) strcpy((dest), (src))
#define strcat_P(dest, src) strcat((dest), (src))
#define strcmp_P(a, b) strcmp((a), (b))

#define true 1
#define false 0

#define MPU6050_ADD        0x68
#define XG_OFFS_TC         0x00
#define YG_OFFS_TC         0x01
#define ZG_OFFS_TC         0x02
#define X_FINE_GAIN        0x03
#define Y_FINE_GAIN        0x04
#define Z_FINE_GAIN        0x05
#define XA_OFFS_H          0x06
#define XA_OFFS_L_TC       0x07
#define YA_OFFS_H          0x08
#define YA_OFFS_L_TC       0x09
#define ZA_OFFS_H          0x0A
#define ZA_OFFS_L_TC       0x0B
#define XG_OFFS_USRH       0x13
#define XG_OFFS_USRL       0x14
#define YG_OFFS_USRH       0x15
#define YG_OFFS_USRL       0x16
#define ZG_OFFS_USRH       0x17
#define ZG_OFFS_USRL       0x18
#define SMPLRT_DIV         0x19
#define CONFIG             0x1A
#define GYRO_CONFIG        0x1B
#define ACCEL_CONFIG       0x1C
#define FF_THR             0x1D
#define FF_DUR             0x1E
#define MOT_THR            0x1F
#define MOT_DUR            0x20
#define ZRMOT_THR          0x21
#define ZRMOT_DUR          0x22
#define FIFO_EN            0x23
#define I2C_MST_CTRL       0x24
#define I2C_SLV0_ADDR      0x25
#define I2C_SLV0_REG       0x26
#define I2C_SLV0_CTRL      0x27
#define I2C_SLV1_ADDR      0x28
#define I2C_SLV1_REG       0x29
#define I2C_SLV1_CTRL      0x2A
#define I2C_SLV2_ADDR      0x2B
#define I2C_SLV2_REG       0x2C
#define I2C_SLV2_CTRL      0x2D
#define I2C_SLV3_ADDR      0x2E
#define I2C_SLV3_REG       0x2F
#define I2C_SLV3_CTRL      0x30
#define I2C_SLV4_ADDR      0x31
#define I2C_SLV4_REG       0x32
#define I2C_SLV4_DO        0x33
#define I2C_SLV4_CTRL      0x34
#define I2C_SLV4_DI        0x35
#define I2C_MST_STATUS     0x36
#define INT_PIN_CFG        0x37
#define INT_ENABLE         0x38
#define DMP_INT_STATUS     0x39
#define INT_STATUS         0x3A
#define ACCEL_XOUT_H       0x3B
#define ACCEL_XOUT_L       0x3C
#define ACCEL_YOUT_H       0x3D
#define ACCEL_YOUT_L       0x3E
#define ACCEL_ZOUT_H       0x3F
#define ACCEL_ZOUT_L       0x40
#define TEMP_OUT_H         0x41
#define TEMP_OUT_L         0x42
#define GYRO_XOUT_H        0x43
#define GYRO_XOUT_L        0x44
#define GYRO_YOUT_H        0x45
#define GYRO_YOUT_L        0x46
#define GYRO_ZOUT_H        0x47
#define GYRO_ZOUT_L        0x48
#define EXT_SENS_DATA_00   0x49
#define EXT_SENS_DATA_01   0x4A
#define EXT_SENS_DATA_02   0x4B
#define EXT_SENS_DATA_03   0x4C
#define EXT_SENS_DATA_04   0x4D
#define EXT_SENS_DATA_05   0x4E
#define EXT_SENS_DATA_06   0x4F
#define EXT_SENS_DATA_07   0x50
#define EXT_SENS_DATA_08   0x51
#define EXT_SENS_DATA_09   0x52
#define EXT_SENS_DATA_10   0x53
#define EXT_SENS_DATA_11   0x54
#define EXT_SENS_DATA_12   0x55
#define EXT_SENS_DATA_13   0x56
#define EXT_SENS_DATA_14   0x57
#define EXT_SENS_DATA_15   0x58
#define EXT_SENS_DATA_16   0x59
#define EXT_SENS_DATA_17   0x5A
#define EXT_SENS_DATA_18   0x5B
#define EXT_SENS_DATA_19   0x5C
#define EXT_SENS_DATA_20   0x5D
#define EXT_SENS_DATA_21   0x5E
#define EXT_SENS_DATA_22   0x5F
#define EXT_SENS_DATA_23   0x60
#define MOT_DETECT_STATUS  0x61
#define I2C_SLV0_DO        0x63
#define I2C_SLV1_DO        0x64
#define I2C_SLV2_DO        0x65
#define I2C_SLV3_DO        0x66
#define I2C_MST_DELAY_CTRL 0x67
#define SIGNAL_PATH_RESET  0x68
#define MOT_DETECT_CTRL    0x69
#define USER_CTRL          0x6A
#define PWR_MGMT_1         0x6B
#define PWR_MGMT_2         0x6C
#define BANK_SEL           0x6D
#define MEM_START_ADDR     0x6E
#define MEM_R_W            0x6F
#define DMP_CFG_1          0x70
#define DMP_CFG_2          0x71
#define FIFO_COUNTH        0x72
#define FIFO_COUNTL        0x73
#define FIFO_R_W           0x74
#define WHO_AM_I           0x75




#define M_PI           3.14
#ifndef MPU6050_DMP_FIFO_RATE_DIVISOR
#define MPU6050_DMP_FIFO_RATE_DIVISOR 0x01 // The New instance of the Firmware has this as the default
#endif
struct MPU6050_Base {
              uint8_t *dmpPacketBuffer;
              uint16_t dmpPacketSize;
              uint8_t devAddr;
              uint8_t buffer[14];
              uint32_t fifoTimeout;
};

void MPU6050_Init(void);
void MPU6050_Read(void);
void CheckID(void);
void MPU6050_init_DMP(void);
void MPU6050_setSleepEnabled(bool enabled);
void MPU6050_reset();
uint8_t DMP_Init();
void MPU6050_setMemoryBank(uint8_t bank, bool prefetchEnabled, bool userBank);
void MPU6050_setSlaveAddress(uint8_t num, uint8_t address);
void MPU6050_setI2CMasterModeEnabled(bool enabled);
void MPU6050_setIntEnabled(uint8_t enabled);
bool writeMemoryBlock(const uint8_t *data , uint16_t dataSize, uint8_t bank, uint8_t address, bool verify , bool useProgMem );
bool writeProgMemoryBlock(const uint8_t *data, uint16_t dataSize, uint8_t bank , uint8_t address , bool verify);
void MPU6050_setMemoryStartAddress(uint8_t address);
void MPU6050_setFIFOEnabled(bool enabled);
void MPU6050_resetDMP(void);
void MPU6050_setDMPEnabled(bool enabled);
void MPU6050_resetFIFO(void);
uint8_t MPU6050_getIntStatus();
uint16_t MPU6050_getFIFOCount();
void MPU6050_getFIFOBytes(uint8_t *data, uint8_t length);
uint16_t MPU6050_dmpGetFIFOPacketSize(struct MPU6050_Base *mpu);
void MPU6050_dmpGetQuaternion(struct Quaternion_Base *q, const uint8_t* packet);
void MPU6050_dmpGetAccel(struct VectorInt16_Base *v, const uint8_t* packet);
uint8_t MPU6050_dmpGetQuaternion16(int16_t *data, const uint8_t* packet);
void MPU6050_dmpGetGravity( struct VectorFloat_Base *v,struct Quaternion_Base *q);
void MPU6050_dmpGetLinearAccel(struct VectorInt16_Base *v,struct VectorInt16_Base *vRaw, struct VectorFloat_Base *gravity);
void MPU6050_dmpGetYawPitchRoll(float *data,struct Quaternion_Base *q, struct VectorFloat_Base *gravity);
uint8_t MPU6050_getOTPBankValid();
void MPU6050_CalibrateAccel(uint8_t Loops);
void MPU6050_CalibrateGyro(uint8_t Loops);
void MPU6050_PID(uint8_t ReadAddress, float kP,float kI, uint8_t Loops);
uint8_t MPU6050_getDeviceID();
uint8_t MPU6050_getFullScaleAccelRange();
void MPU6050_ConfigDMP(struct MPU6050_Base *mpu,uint8_t *devStatus,bool *dmpReady,uint8_t *mpuIntStatus,uint16_t *packetSize);
void MPU6050_GetData(struct MPU6050_Base *mpu,bool *dmpReady,volatile bool *mpuInterrupt,uint16_t *packetSize,uint8_t *mpuIntStatus);
#define MPU6050_FIFO_DEFAULT_TIMEOUT 11000




#endif /* USER_MPU6050_H_ */
