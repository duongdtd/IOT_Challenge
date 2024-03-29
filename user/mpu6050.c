/*
 * mpu6050.c
 *
 *  Created on: 11 thg 3, 2022
 *      Author: Admin
 */

#include "MPU6050.h"
#include "i2c_lib.h"
#include "sl_app_log.h"
#include "stdio.h"
#include "test_variable.h"
#include "string.h"
#include "stdlib.h"
#include "math.h"
#include "led_buzzer.h"
#define MPU6050_DMP_CODE_SIZE       1929    // dmpMemory[]
#define MPU6050_DMP_CONFIG_SIZE     192     // dmpConfig[]
#define MPU6050_DMP_UPDATES_SIZE    47      // dmpUpdates[]
#include <inttypes.h>
uint8_t *MPUdmpPacketBuffer;
#define PROGMEM
#define PGM_P  const char *
#define PSTR(str) (str)
#define F(x) x

typedef void prog_void;
typedef char prog_char;
typedef unsigned char prog_uchar;
typedef int8_t prog_int8_t;
typedef uint8_t prog_uint8_t;
typedef int16_t prog_int16_t;
typedef uint16_t prog_uint16_t;
typedef int32_t prog_int32_t;
typedef uint32_t prog_uint32_t;

#define strcpy_P(dest, src) strcpy((dest), (src))
#define strcat_P(dest, src) strcat((dest), (src))
#define strcmp_P(a, b) strcmp((a), (b))

#define pgm_read_byte(addr) (*(const unsigned char *)(addr))
#define pgm_read_word(addr) (*(const unsigned short *)(addr))
#define pgm_read_dword(addr) (*(const unsigned long *)(addr))
#define pgm_read_float(addr) (*(const float *)(addr))

#define pgm_read_byte_near(addr) pgm_read_byte(addr)
#define pgm_read_word_near(addr) pgm_read_word(addr)
#define pgm_read_dword_near(addr) pgm_read_dword(addr)
#define pgm_read_float_near(addr) pgm_read_float(addr)
#define pgm_read_byte_far(addr) pgm_read_byte(addr)
#define pgm_read_word_far(addr) pgm_read_word(addr)
#define pgm_read_dword_far(addr) pgm_read_dword(addr)
#define pgm_read_float_far(addr) pgm_read_float(addr)

#define pgm_read_byte_near(addr) pgm_read_byte(addr)
#define pgm_read_word_near(addr) pgm_read_word(addr)
#define pgm_read_dword_near(addr) pgm_read_dword(addr)
#define pgm_read_float_near(addr) pgm_read_float(addr)
#define pgm_read_byte_far(addr) pgm_read_byte(addr)
#define pgm_read_word_far(addr) pgm_read_word(addr)
#define pgm_read_dword_far(addr) pgm_read_dword(addr)
#define pgm_read_float_far(addr) pgm_read_float(addr)
/* ================================================================================================ *
 | Default MotionApps v2.0 42-byte FIFO packet structure:                                           |
 |                                                                                                  |
 | [QUAT W][      ][QUAT X][      ][QUAT Y][      ][QUAT Z][      ][GYRO X][      ][GYRO Y][      ] |
 |   0   1   2   3   4   5   6   7   8   9  10  11  12  13  14  15  16  17  18  19  20  21  22  23  |
 |                                                                                                  |
 | [GYRO Z][      ][ACC X ][      ][ACC Y ][      ][ACC Z ][      ][      ]                         |
 |  24  25  26  27  28  29  30  31  32  33  34  35  36  37  38  39  40  41                          |
 * ================================================================================================ */

// this block of memory gets written to the MPU on start-up, and it seems
// to be volatile memory, so it has to be done each time (it only takes ~1
// second though)

// I Only Changed this by applying all the configuration data and capturing it before startup:
// *** this is a capture of the DMP Firmware after all the messy changes were made so we can just load it
static const unsigned char dmpMemory[MPU6050_DMP_CODE_SIZE] PROGMEM = {
  /* bank # 0 */
  0xFB, 0x00, 0x00, 0x3E, 0x00, 0x0B, 0x00, 0x36, 0x00, 0x01, 0x00, 0x02, 0x00, 0x03, 0x00, 0x00,
  0x00, 0x65, 0x00, 0x54, 0xFF, 0xEF, 0x00, 0x00, 0xFA, 0x80, 0x00, 0x0B, 0x12, 0x82, 0x00, 0x01,
  0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x28, 0x00, 0x00, 0xFF, 0xFF, 0x45, 0x81, 0xFF, 0xFF, 0xFA, 0x72, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x03, 0xE8, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x7F, 0xFF, 0xFF, 0xFE, 0x80, 0x01,
  0x00, 0x1B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x40, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x02, 0xCB, 0x47, 0xA2, 0x20, 0x00, 0x00, 0x00,
  0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x60, 0x00, 0x00, 0x00,
  0x41, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x0B, 0x2A, 0x00, 0x00, 0x16, 0x55, 0x00, 0x00, 0x21, 0x82,
  0xFD, 0x87, 0x26, 0x50, 0xFD, 0x80, 0x00, 0x00, 0x00, 0x1F, 0x00, 0x00, 0x00, 0x05, 0x80, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00,
  0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x6F, 0x00, 0x02, 0x65, 0x32, 0x00, 0x00, 0x5E, 0xC0,
  0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0xFB, 0x8C, 0x6F, 0x5D, 0xFD, 0x5D, 0x08, 0xD9, 0x00, 0x7C, 0x73, 0x3B, 0x00, 0x6C, 0x12, 0xCC,
  0x32, 0x00, 0x13, 0x9D, 0x32, 0x00, 0xD0, 0xD6, 0x32, 0x00, 0x08, 0x00, 0x40, 0x00, 0x01, 0xF4,
  0xFF, 0xE6, 0x80, 0x79, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0xD0, 0xD6, 0x00, 0x00, 0x27, 0x10,
  /* bank # 1 */
  0xFB, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00,
  0x00, 0x00, 0xFA, 0x36, 0xFF, 0xBC, 0x30, 0x8E, 0x00, 0x05, 0xFB, 0xF0, 0xFF, 0xD9, 0x5B, 0xC8,
  0xFF, 0xD0, 0x9A, 0xBE, 0x00, 0x00, 0x10, 0xA9, 0xFF, 0xF4, 0x1E, 0xB2, 0x00, 0xCE, 0xBB, 0xF7,
  0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x04, 0x00, 0x02, 0x00, 0x02, 0x02, 0x00, 0x00, 0x0C,
  0xFF, 0xC2, 0x80, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0xCF, 0x80, 0x00, 0x40, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x14,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x09, 0x23, 0xA1, 0x35, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x03, 0x3F, 0x68, 0xB6, 0x79, 0x35, 0x28, 0xBC, 0xC6, 0x7E, 0xD1, 0x6C,
  0x80, 0x00, 0xFF, 0xFF, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0xB2, 0x6A, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3F, 0xF0, 0x00, 0x00, 0x00, 0x30,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00,
  0x00, 0x00, 0x25, 0x4D, 0x00, 0x2F, 0x70, 0x6D, 0x00, 0x00, 0x05, 0xAE, 0x00, 0x0C, 0x02, 0xD0,
  /* bank # 2 */
  0x00, 0x00, 0x00, 0x00, 0x00, 0x65, 0x00, 0x54, 0xFF, 0xEF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x01, 0x00, 0x00, 0x44, 0x00, 0x01, 0x00, 0x05, 0x8B, 0xC1, 0x00, 0x00, 0x01, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x65, 0x00, 0x00, 0x00, 0x54, 0x00, 0x00, 0xFF, 0xEF, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x1B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x1B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* bank # 3 */
  0xD8, 0xDC, 0xBA, 0xA2, 0xF1, 0xDE, 0xB2, 0xB8, 0xB4, 0xA8, 0x81, 0x91, 0xF7, 0x4A, 0x90, 0x7F,
  0x91, 0x6A, 0xF3, 0xF9, 0xDB, 0xA8, 0xF9, 0xB0, 0xBA, 0xA0, 0x80, 0xF2, 0xCE, 0x81, 0xF3, 0xC2,
  0xF1, 0xC1, 0xF2, 0xC3, 0xF3, 0xCC, 0xA2, 0xB2, 0x80, 0xF1, 0xC6, 0xD8, 0x80, 0xBA, 0xA7, 0xDF,
  0xDF, 0xDF, 0xF2, 0xA7, 0xC3, 0xCB, 0xC5, 0xB6, 0xF0, 0x87, 0xA2, 0x94, 0x24, 0x48, 0x70, 0x3C,
  0x95, 0x40, 0x68, 0x34, 0x58, 0x9B, 0x78, 0xA2, 0xF1, 0x83, 0x92, 0x2D, 0x55, 0x7D, 0xD8, 0xB1,
  0xB4, 0xB8, 0xA1, 0xD0, 0x91, 0x80, 0xF2, 0x70, 0xF3, 0x70, 0xF2, 0x7C, 0x80, 0xA8, 0xF1, 0x01,
  0xB0, 0x98, 0x87, 0xD9, 0x43, 0xD8, 0x86, 0xC9, 0x88, 0xBA, 0xA1, 0xF2, 0x0E, 0xB8, 0x97, 0x80,
  0xF1, 0xA9, 0xDF, 0xDF, 0xDF, 0xAA, 0xDF, 0xDF, 0xDF, 0xF2, 0xAA, 0x4C, 0xCD, 0x6C, 0xA9, 0x0C,
  0xC9, 0x2C, 0x97, 0x97, 0x97, 0x97, 0xF1, 0xA9, 0x89, 0x26, 0x46, 0x66, 0xB0, 0xB4, 0xBA, 0x80,
  0xAC, 0xDE, 0xF2, 0xCA, 0xF1, 0xB2, 0x8C, 0x02, 0xA9, 0xB6, 0x98, 0x00, 0x89, 0x0E, 0x16, 0x1E,
  0xB8, 0xA9, 0xB4, 0x99, 0x2C, 0x54, 0x7C, 0xB0, 0x8A, 0xA8, 0x96, 0x36, 0x56, 0x76, 0xF1, 0xB9,
  0xAF, 0xB4, 0xB0, 0x83, 0xC0, 0xB8, 0xA8, 0x97, 0x11, 0xB1, 0x8F, 0x98, 0xB9, 0xAF, 0xF0, 0x24,
  0x08, 0x44, 0x10, 0x64, 0x18, 0xF1, 0xA3, 0x29, 0x55, 0x7D, 0xAF, 0x83, 0xB5, 0x93, 0xAF, 0xF0,
  0x00, 0x28, 0x50, 0xF1, 0xA3, 0x86, 0x9F, 0x61, 0xA6, 0xDA, 0xDE, 0xDF, 0xD9, 0xFA, 0xA3, 0x86,
  0x96, 0xDB, 0x31, 0xA6, 0xD9, 0xF8, 0xDF, 0xBA, 0xA6, 0x8F, 0xC2, 0xC5, 0xC7, 0xB2, 0x8C, 0xC1,
  0xB8, 0xA2, 0xDF, 0xDF, 0xDF, 0xA3, 0xDF, 0xDF, 0xDF, 0xD8, 0xD8, 0xF1, 0xB8, 0xA8, 0xB2, 0x86,
  /* bank # 4 */
  0xB4, 0x98, 0x0D, 0x35, 0x5D, 0xB8, 0xAA, 0x98, 0xB0, 0x87, 0x2D, 0x35, 0x3D, 0xB2, 0xB6, 0xBA,
  0xAF, 0x8C, 0x96, 0x19, 0x8F, 0x9F, 0xA7, 0x0E, 0x16, 0x1E, 0xB4, 0x9A, 0xB8, 0xAA, 0x87, 0x2C,
  0x54, 0x7C, 0xB9, 0xA3, 0xDE, 0xDF, 0xDF, 0xA3, 0xB1, 0x80, 0xF2, 0xC4, 0xCD, 0xC9, 0xF1, 0xB8,
  0xA9, 0xB4, 0x99, 0x83, 0x0D, 0x35, 0x5D, 0x89, 0xB9, 0xA3, 0x2D, 0x55, 0x7D, 0xB5, 0x93, 0xA3,
  0x0E, 0x16, 0x1E, 0xA9, 0x2C, 0x54, 0x7C, 0xB8, 0xB4, 0xB0, 0xF1, 0x97, 0x83, 0xA8, 0x11, 0x84,
  0xA5, 0x09, 0x98, 0xA3, 0x83, 0xF0, 0xDA, 0x24, 0x08, 0x44, 0x10, 0x64, 0x18, 0xD8, 0xF1, 0xA5,
  0x29, 0x55, 0x7D, 0xA5, 0x85, 0x95, 0x02, 0x1A, 0x2E, 0x3A, 0x56, 0x5A, 0x40, 0x48, 0xF9, 0xF3,
  0xA3, 0xD9, 0xF8, 0xF0, 0x98, 0x83, 0x24, 0x08, 0x44, 0x10, 0x64, 0x18, 0x97, 0x82, 0xA8, 0xF1,
  0x11, 0xF0, 0x98, 0xA2, 0x24, 0x08, 0x44, 0x10, 0x64, 0x18, 0xDA, 0xF3, 0xDE, 0xD8, 0x83, 0xA5,
  0x94, 0x01, 0xD9, 0xA3, 0x02, 0xF1, 0xA2, 0xC3, 0xC5, 0xC7, 0xD8, 0xF1, 0x84, 0x92, 0xA2, 0x4D,
  0xDA, 0x2A, 0xD8, 0x48, 0x69, 0xD9, 0x2A, 0xD8, 0x68, 0x55, 0xDA, 0x32, 0xD8, 0x50, 0x71, 0xD9,
  0x32, 0xD8, 0x70, 0x5D, 0xDA, 0x3A, 0xD8, 0x58, 0x79, 0xD9, 0x3A, 0xD8, 0x78, 0x93, 0xA3, 0x4D,
  0xDA, 0x2A, 0xD8, 0x48, 0x69, 0xD9, 0x2A, 0xD8, 0x68, 0x55, 0xDA, 0x32, 0xD8, 0x50, 0x71, 0xD9,
  0x32, 0xD8, 0x70, 0x5D, 0xDA, 0x3A, 0xD8, 0x58, 0x79, 0xD9, 0x3A, 0xD8, 0x78, 0xA8, 0x8A, 0x9A,
  0xF0, 0x28, 0x50, 0x78, 0x9E, 0xF3, 0x88, 0x18, 0xF1, 0x9F, 0x1D, 0x98, 0xA8, 0xD9, 0x08, 0xD8,
  0xC8, 0x9F, 0x12, 0x9E, 0xF3, 0x15, 0xA8, 0xDA, 0x12, 0x10, 0xD8, 0xF1, 0xAF, 0xC8, 0x97, 0x87,
  /* bank # 5 */
  0x34, 0xB5, 0xB9, 0x94, 0xA4, 0x21, 0xF3, 0xD9, 0x22, 0xD8, 0xF2, 0x2D, 0xF3, 0xD9, 0x2A, 0xD8,
  0xF2, 0x35, 0xF3, 0xD9, 0x32, 0xD8, 0x81, 0xA4, 0x60, 0x60, 0x61, 0xD9, 0x61, 0xD8, 0x6C, 0x68,
  0x69, 0xD9, 0x69, 0xD8, 0x74, 0x70, 0x71, 0xD9, 0x71, 0xD8, 0xB1, 0xA3, 0x84, 0x19, 0x3D, 0x5D,
  0xA3, 0x83, 0x1A, 0x3E, 0x5E, 0x93, 0x10, 0x30, 0x81, 0x10, 0x11, 0xB8, 0xB0, 0xAF, 0x8F, 0x94,
  0xF2, 0xDA, 0x3E, 0xD8, 0xB4, 0x9A, 0xA8, 0x87, 0x29, 0xDA, 0xF8, 0xD8, 0x87, 0x9A, 0x35, 0xDA,
  0xF8, 0xD8, 0x87, 0x9A, 0x3D, 0xDA, 0xF8, 0xD8, 0xB1, 0xB9, 0xA4, 0x98, 0x85, 0x02, 0x2E, 0x56,
  0xA5, 0x81, 0x00, 0x0C, 0x14, 0xA3, 0x97, 0xB0, 0x8A, 0xF1, 0x2D, 0xD9, 0x28, 0xD8, 0x4D, 0xD9,
  0x48, 0xD8, 0x6D, 0xD9, 0x68, 0xD8, 0xB1, 0x84, 0x0D, 0xDA, 0x0E, 0xD8, 0xA3, 0x29, 0x83, 0xDA,
  0x2C, 0x0E, 0xD8, 0xA3, 0x84, 0x49, 0x83, 0xDA, 0x2C, 0x4C, 0x0E, 0xD8, 0xB8, 0xB0, 0xA8, 0x8A,
  0x9A, 0xF5, 0x20, 0xAA, 0xDA, 0xDF, 0xD8, 0xA8, 0x40, 0xAA, 0xD0, 0xDA, 0xDE, 0xD8, 0xA8, 0x60,
  0xAA, 0xDA, 0xD0, 0xDF, 0xD8, 0xF1, 0x97, 0x86, 0xA8, 0x31, 0x9B, 0x06, 0x99, 0x07, 0xAB, 0x97,
  0x28, 0x88, 0x9B, 0xF0, 0x0C, 0x20, 0x14, 0x40, 0xB8, 0xB0, 0xB4, 0xA8, 0x8C, 0x9C, 0xF0, 0x04,
  0x28, 0x51, 0x79, 0x1D, 0x30, 0x14, 0x38, 0xB2, 0x82, 0xAB, 0xD0, 0x98, 0x2C, 0x50, 0x50, 0x78,
  0x78, 0x9B, 0xF1, 0x1A, 0xB0, 0xF0, 0x8A, 0x9C, 0xA8, 0x29, 0x51, 0x79, 0x8B, 0x29, 0x51, 0x79,
  0x8A, 0x24, 0x70, 0x59, 0x8B, 0x20, 0x58, 0x71, 0x8A, 0x44, 0x69, 0x38, 0x8B, 0x39, 0x40, 0x68,
  0x8A, 0x64, 0x48, 0x31, 0x8B, 0x30, 0x49, 0x60, 0xA5, 0x88, 0x20, 0x09, 0x71, 0x58, 0x44, 0x68,
  /* bank # 6 */
  0x11, 0x39, 0x64, 0x49, 0x30, 0x19, 0xF1, 0xAC, 0x00, 0x2C, 0x54, 0x7C, 0xF0, 0x8C, 0xA8, 0x04,
  0x28, 0x50, 0x78, 0xF1, 0x88, 0x97, 0x26, 0xA8, 0x59, 0x98, 0xAC, 0x8C, 0x02, 0x26, 0x46, 0x66,
  0xF0, 0x89, 0x9C, 0xA8, 0x29, 0x51, 0x79, 0x24, 0x70, 0x59, 0x44, 0x69, 0x38, 0x64, 0x48, 0x31,
  0xA9, 0x88, 0x09, 0x20, 0x59, 0x70, 0xAB, 0x11, 0x38, 0x40, 0x69, 0xA8, 0x19, 0x31, 0x48, 0x60,
  0x8C, 0xA8, 0x3C, 0x41, 0x5C, 0x20, 0x7C, 0x00, 0xF1, 0x87, 0x98, 0x19, 0x86, 0xA8, 0x6E, 0x76,
  0x7E, 0xA9, 0x99, 0x88, 0x2D, 0x55, 0x7D, 0x9E, 0xB9, 0xA3, 0x8A, 0x22, 0x8A, 0x6E, 0x8A, 0x56,
  0x8A, 0x5E, 0x9F, 0xB1, 0x83, 0x06, 0x26, 0x46, 0x66, 0x0E, 0x2E, 0x4E, 0x6E, 0x9D, 0xB8, 0xAD,
  0x00, 0x2C, 0x54, 0x7C, 0xF2, 0xB1, 0x8C, 0xB4, 0x99, 0xB9, 0xA3, 0x2D, 0x55, 0x7D, 0x81, 0x91,
  0xAC, 0x38, 0xAD, 0x3A, 0xB5, 0x83, 0x91, 0xAC, 0x2D, 0xD9, 0x28, 0xD8, 0x4D, 0xD9, 0x48, 0xD8,
  0x6D, 0xD9, 0x68, 0xD8, 0x8C, 0x9D, 0xAE, 0x29, 0xD9, 0x04, 0xAE, 0xD8, 0x51, 0xD9, 0x04, 0xAE,
  0xD8, 0x79, 0xD9, 0x04, 0xD8, 0x81, 0xF3, 0x9D, 0xAD, 0x00, 0x8D, 0xAE, 0x19, 0x81, 0xAD, 0xD9,
  0x01, 0xD8, 0xF2, 0xAE, 0xDA, 0x26, 0xD8, 0x8E, 0x91, 0x29, 0x83, 0xA7, 0xD9, 0xAD, 0xAD, 0xAD,
  0xAD, 0xF3, 0x2A, 0xD8, 0xD8, 0xF1, 0xB0, 0xAC, 0x89, 0x91, 0x3E, 0x5E, 0x76, 0xF3, 0xAC, 0x2E,
  0x2E, 0xF1, 0xB1, 0x8C, 0x5A, 0x9C, 0xAC, 0x2C, 0x28, 0x28, 0x28, 0x9C, 0xAC, 0x30, 0x18, 0xA8,
  0x98, 0x81, 0x28, 0x34, 0x3C, 0x97, 0x24, 0xA7, 0x28, 0x34, 0x3C, 0x9C, 0x24, 0xF2, 0xB0, 0x89,
  0xAC, 0x91, 0x2C, 0x4C, 0x6C, 0x8A, 0x9B, 0x2D, 0xD9, 0xD8, 0xD8, 0x51, 0xD9, 0xD8, 0xD8, 0x79,
  /* bank # 7 */
  0xD9, 0xD8, 0xD8, 0xF1, 0x9E, 0x88, 0xA3, 0x31, 0xDA, 0xD8, 0xD8, 0x91, 0x2D, 0xD9, 0x28, 0xD8,
  0x4D, 0xD9, 0x48, 0xD8, 0x6D, 0xD9, 0x68, 0xD8, 0xB1, 0x83, 0x93, 0x35, 0x3D, 0x80, 0x25, 0xDA,
  0xD8, 0xD8, 0x85, 0x69, 0xDA, 0xD8, 0xD8, 0xB4, 0x93, 0x81, 0xA3, 0x28, 0x34, 0x3C, 0xF3, 0xAB,
  0x8B, 0xF8, 0xA3, 0x91, 0xB6, 0x09, 0xB4, 0xD9, 0xAB, 0xDE, 0xFA, 0xB0, 0x87, 0x9C, 0xB9, 0xA3,
  0xDD, 0xF1, 0x20, 0x28, 0x30, 0x38, 0x9A, 0xF1, 0x28, 0x30, 0x38, 0x9D, 0xF1, 0xA3, 0xA3, 0xA3,
  0xA3, 0xF2, 0xA3, 0xB4, 0x90, 0x80, 0xF2, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3,
  0xA3, 0xB2, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xB0, 0x87, 0xB5, 0x99, 0xF1, 0x28, 0x30, 0x38,
  0x98, 0xF1, 0xA3, 0xA3, 0xA3, 0xA3, 0x97, 0xA3, 0xA3, 0xA3, 0xA3, 0xF3, 0x9B, 0xA3, 0x30, 0xDC,
  0xB9, 0xA7, 0xF1, 0x26, 0x26, 0x26, 0xFE, 0xD8, 0xFF,

};
void MPU6050_init_DMP()
{
  i2c_writeBits(MPU6050_ADD, PWR_MGMT_1, 2, 3, 0x01);
  i2c_writeByte(MPU6050_ADD,ACCEL_CONFIG,0x00);//full scale range mode 0 +- 2g
  i2c_writeByte(MPU6050_ADD,GYRO_CONFIG,0x00);//full scale range mode 0 +- 250do/s
  MPU6050_setSleepEnabled(false);

}
uint8_t MPU6050_getFullScaleAccelRange() {
  uint8_t data;
  i2c_readBits(MPU6050_ADD, ACCEL_CONFIG, 4, 2, &data);
  return data;
}
void MPU6050_setFIFOEnabled(bool enabled) {
    i2c_writeBit(MPU6050_ADD, USER_CTRL, 6, enabled);
}
void MPU6050_resetDMP() {
  i2c_writeBit(MPU6050_ADD, USER_CTRL, 3, true);
}
void MPU6050_setDMPEnabled(bool enabled) {
    i2c_writeBit(MPU6050_ADD, USER_CTRL, 7, enabled);
}
void MPU6050_setSleepEnabled(bool enabled) {
    i2c_writeBit(MPU6050_ADD, PWR_MGMT_1, 6, enabled);
}
void MPU6050_reset(){
    i2c_writeBit(MPU6050_ADD, PWR_MGMT_1, 7, true);
}
uint8_t MPU6050_getDeviceID() {
    uint8_t data;
    i2c_readBits(MPU6050_ADD, WHO_AM_I, 6, 6, &data);
    return data;
}
void MPU6050_setMemoryBank(uint8_t bank, bool prefetchEnabled, bool userBank) {
    bank &= 0x1F;
    if (userBank) bank |= 0x20;
    if (prefetchEnabled) bank |= 0x40;
    i2c_writeByte(MPU6050_ADD, 0x6D, bank);
}
void MPU6050_setSlaveAddress(uint8_t num, uint8_t address) {
    if (num > 3) { return; }
    i2c_writeByte(MPU6050_ADD, I2C_SLV0_ADDR + num*3, address);
}
void MPU6050_setI2CMasterModeEnabled(bool enabled) {
    i2c_writeBit(MPU6050_ADD, USER_CTRL, 5, enabled);
}
void MPU6050_setIntEnabled(uint8_t enabled) {
    i2c_writeByte(MPU6050_ADD, INT_ENABLE, enabled);
}
void MPU6050_setMemoryStartAddress(uint8_t address)
{
  i2c_writeByte(MPU6050_ADD,MEM_START_ADDR,address);
  }
void MPU6050_resetFIFO() {
    i2c_writeBit(MPU6050_ADD, USER_CTRL, 2, true);
}
bool writeMemoryBlock(const uint8_t *data, uint16_t dataSize, uint8_t bank, uint8_t address, bool verify, bool useProgMem)
{
      MPU6050_setMemoryBank(bank, false, false);
      MPU6050_setMemoryStartAddress(address);
      uint8_t chunkSize;
      uint8_t *verifyBuffer;
      uint8_t *progBuffer;
      uint16_t i;
      uint8_t j;
      if (verify) { verifyBuffer = (uint8_t *)malloc(16);}
      if (useProgMem){ progBuffer = (uint8_t *)malloc(16);}
      for (i = 0; i < dataSize;) {
          // determine correct chunk size according to bank position and data size
          chunkSize = 16;

          // make sure we don't go past the data size
          if (i + chunkSize > dataSize) {chunkSize = dataSize - i;}

          // make sure this chunk doesn't go past the bank boundary (256 bytes)
          if (chunkSize > 256 - address) {chunkSize = 256 - address;}

          if (useProgMem) {
              // write the chunk of data as specified
              for (j = 0; j < chunkSize; j++){ progBuffer[j] = pgm_read_byte(data + i + j);}
          } else {
              // write the chunk of data as specified
              progBuffer = (uint8_t *)data + i;
          }
          i2c_write_nBytes(MPU6050_ADD, MEM_R_W, progBuffer, chunkSize);


          // verify data if needed
          if (verify && verifyBuffer) {
              MPU6050_setMemoryBank(bank, false, false);
              MPU6050_setMemoryStartAddress(address);
              i2c_read(MPU6050_ADD, MEM_R_W, verifyBuffer, chunkSize);
              if (memcmp(progBuffer, verifyBuffer, chunkSize) != 0) {
                  app_log("\n");
                  free(verifyBuffer);
                  if (useProgMem) {free(progBuffer); }
                  return false;
              }
          }

          // increase byte index by [chunkSize]
          i += chunkSize;

          // uint8_t automatically wraps to 0 at 256
          address += chunkSize;

          // if we aren't done, update bank (if necessary) and address
          if (i < dataSize) {
              if (address == 0) bank++;
              MPU6050_setMemoryBank(bank, false, false);
              MPU6050_setMemoryStartAddress(address);
          }
      }
      if (verify){ free(verifyBuffer);}
      if (useProgMem){ free(progBuffer);}
      return true;

}
bool writeProgMemoryBlock(const uint8_t *data, uint16_t dataSize, uint8_t bank, uint8_t address, bool verify) {
    return writeMemoryBlock(data, dataSize, bank, address, verify, true);
}
uint16_t MPU6050_dmpGetFIFOPacketSize(struct MPU6050_Base *mpu) {
    return mpu->dmpPacketSize;
}
uint8_t MPU6050_getIntStatus() {
  uint8_t buffer;
    i2c_read(MPU6050_ADD, INT_STATUS, &buffer, 1);
    return buffer;
}
uint8_t MPU6050_readMemoryByte() {
    uint8_t buffer;
    i2c_read(MPU6050_ADD, MEM_R_W, &buffer, 1);
    return buffer;
}

uint16_t MPU6050_getFIFOCount(){

  uint8_t buffer_1,buffer_2;
  uint16_t data;
  i2c_read(MPU6050_ADD, 0x72, &buffer_1,1);
  i2c_read(MPU6050_ADD, 0x73,&buffer_2,1);
  data = (uint16_t)(buffer_1 << 8 | buffer_2);
  return data;
}
void MPU6050_getFIFOBytes(uint8_t *data, uint8_t length) {
    i2c_read(MPU6050_ADD, FIFO_R_W, data, length);
}
uint8_t MPU6050_getOTPBankValid() {
    uint8_t data;
    i2c_read(MPU6050_ADD, 0x00,&data , 1);
    data = data & (1 << 0);
    return data;
}
uint8_t DMP_Init()
{
  app_log("\n\nResetting MPU6050...\n");
  MPU6050_reset();
  sl_sleeptimer_delay_millisecond(30);
  //
  MPU6050_setSleepEnabled(0);
  MPU6050_setMemoryBank(0x10, 1, 1);
  i2c_writeByte(MPU6050_ADD, 0x6E, 0x06);
  app_log("Checking hardware revision...\n");
  app_log("Revision @ user[16][6] = ");
  app_log("%d\n",MPU6050_readMemoryByte());
  MPU6050_setMemoryBank(0, 0, 0);
  // check OTP bank valid
  app_log("Reading OTP bank valid flag...\n");
  app_log("OTP bank is :");
  app_log(MPU6050_getOTPBankValid() ? F("valid!\n") : F("invalid!\n"));
  app_log("Setting slave 0 address to 0x7F...\n");
  MPU6050_setSlaveAddress(0, 0x07);
  MPU6050_setI2CMasterModeEnabled (false);
  app_log("Setting slave 0 address to 0x68 (self)...\n");
  MPU6050_setSlaveAddress(0, 0x68);
  app_log("Resetting I2C Master control...\n");
  i2c_writeBit(MPU6050_ADD, USER_CTRL, 1, true);
  app_log("Setting clock source to Z Gyro...\n");
  i2c_writeBits(MPU6050_ADD, PWR_MGMT_1, 2, 3, 0x03);

  app_log("Setting DMP and FIFO_OFLOW interrupts enabled...\n");
  MPU6050_setIntEnabled ((1<<4|1<<1));
  app_log("Setting sample rate to 200Hz...\n");
  i2c_writeByte(MPU6050_ADD, SMPLRT_DIV, 4);
  app_log("Setting external frame sync to TEMP_OUT_L[0]...\n");
  i2c_writeBits (MPU6050_ADD, CONFIG, 5, 3, 0x1);
  app_log("Setting DLPF bandwidth to 42Hz...\n");
  i2c_writeBits (MPU6050_ADD, CONFIG, 2, 3, 0x03);
  app_log("Setting gyro sensitivity to +/- 2000 deg/sec...\n");
  i2c_writeByte(MPU6050_ADD, GYRO_CONFIG, 0x03); //full scale range mode 0 +- 2000do/s
  // load DMP code into memory banks
  if (!writeProgMemoryBlock(dmpMemory, MPU6050_DMP_CODE_SIZE,0,0,true))
    {return 1;}
  app_log("Success! DMP code written and verified.\n");
  // Set the FIFO Rate Divisor int the DMP Firmware Memory
  unsigned char dmpUpdate[] ={ 0x00, 0x01 };
  writeMemoryBlock (dmpUpdate, 0x02, 0x02, 0x16,true,false);
  //write start address MSB into register
  i2c_writeByte(MPU6050_ADD,DMP_CFG_1,0x03);
  //write start address LSB into register
  i2c_writeByte (MPU6050_ADD, DMP_CFG_2, 0x00);
  app_log("Clearing OTP Bank flag...\n");
  i2c_writeBit(MPU6050_ADD, XG_OFFS_TC, 0, false);
  app_log("Setting motion detection threshold to 2...\n");
  i2c_writeByte(MPU6050_ADD,MOT_THR,2);
  app_log("Setting zero-motion detection threshold to 156...\n");
  i2c_writeByte(MPU6050_ADD,ZRMOT_THR,156);
  app_log("Setting motion detection duration to 80...\n");
  i2c_writeByte(MPU6050_ADD, MOT_DUR, 80);
  app_log("Setting zero-motion detection duration to 0...");
  i2c_writeByte(MPU6050_ADD,ZRMOT_DUR,0);
  app_log ("Enabling FIFO...\n");
  MPU6050_setFIFOEnabled(true);

  app_log ("Resetting DMP...\n");
  MPU6050_resetDMP();

  app_log("DMP is good to go! Finally.\n");

  app_log("Disabling DMP (you turn it on later)...\n");
  MPU6050_setDMPEnabled(false);

  app_log ("Setting up internal 42-byte (default) DMP packet buffer...\n");

  app_log("Resetting FIFO and clearing INT status one last time...\n");
  MPU6050_resetFIFO();
  app_log("%d\n",MPU6050_getIntStatus());
  return 0; // success
}
void MPU6050_Init(void)
{
  i2c_writeByte(MPU6050_ADD, SMPLRT_DIV, 0x07);//set sample rate to 8000/(1+7) = 1000Hz
  i2c_writeByte(MPU6050_ADD, PWR_MGMT_1, 0x00);// wake up MPU6050
  //i2c_write(MPU6050_ADD,CONFIG,0x00);//disable DLPF
  i2c_writeByte(MPU6050_ADD,ACCEL_CONFIG,0x00);//full scale range mode 0 +- 2g
  i2c_writeByte(MPU6050_ADD,GYRO_CONFIG,0x00);//full scale range mode 0 +- 250do/s
  //i2c_write(MPU6050_ADD,0x74,0x06);//disable sensor output to FIFO buffer
}
void MPU6050_Read(void)
{
  int16_t Ax,Ay,Az;
  int16_t Gx,Gy,Gz;
  // Prepare For Reading, Starting From ACCEL_XOUT_H
  uint8_t data_ax_h,data_ax_l,data_ay_h,data_ay_l,data_az_h,data_az_l;
  uint8_t data_gx_h,data_gx_l,data_gy_h,data_gy_l,data_gz_h,data_gz_l;
  //read data accelerometer
  i2c_read(MPU6050_ADD, ACCEL_XOUT_H, &data_ax_h, 1);
  i2c_read(MPU6050_ADD, ACCEL_XOUT_L, &data_ax_l, 1);
  Ax = (int16_t)(data_ax_h << 8 | data_ax_l);
  i2c_read(MPU6050_ADD, ACCEL_YOUT_H, &data_ay_h, 1);
  i2c_read(MPU6050_ADD, ACCEL_YOUT_L, &data_ay_l, 1);
  Ay = (int16_t)(data_ay_h << 8 | data_ay_l);
  i2c_read(MPU6050_ADD, ACCEL_ZOUT_H, &data_az_h, 1);
  i2c_read(MPU6050_ADD, ACCEL_ZOUT_L, &data_az_l, 1);
  Az = (int16_t)(data_az_h << 8 | data_az_l);
  // read data gyroscope
  i2c_read(MPU6050_ADD, GYRO_XOUT_H, &data_gx_h, 1);
  i2c_read(MPU6050_ADD, GYRO_XOUT_L, &data_gx_l, 1);
  Gx = (int16_t)(data_gx_h << 8 | data_gx_l);
  i2c_read(MPU6050_ADD, GYRO_YOUT_H, &data_gy_h, 1);
  i2c_read(MPU6050_ADD, GYRO_YOUT_L, &data_gy_l, 1);
  Gy = (int16_t)(data_gy_h << 8 | data_gy_l);
  i2c_read(MPU6050_ADD, GYRO_ZOUT_H, &data_gz_h, 1);
  i2c_read(MPU6050_ADD, GYRO_ZOUT_L, &data_gz_l, 1);
  Gz = (int16_t)(data_gz_h << 8 | data_gz_l);
  //log data accelerometer
  app_log("Ax :%d", Ax);
  app_log(" | Ay :%d", Ay);
  app_log(" | Az :%d", Az);
  //log data accelerometer
  app_log(" | Gx :%d", Gx);
  app_log(" | Gy :%d", Gy);
  app_log(" | Gz :%d", Gz);
  app_log("\n");
}
void CheckID(void)
{
    uint8_t data;
    i2c_read(0x68, 0x75, &data, 1);
    app_log("check : %d\n",data);
  }
uint8_t MPU6050_dmpGetQuaternion16(int16_t *data, const uint8_t* packet) {
    // TODO: accommodate different arrangements of sent data (ONLY default supported now)
    if (packet == 0) packet = MPUdmpPacketBuffer;
    data[0] = ((packet[0] << 8) | packet[1]);
    data[1] = ((packet[4] << 8) | packet[5]);
    data[2] = ((packet[8] << 8) | packet[9]);
    data[3] = ((packet[12] << 8) | packet[13]);
    return 0;
}
void MPU6050_dmpGetQuaternion(struct Quaternion_Base *q, const uint8_t* packet) {
    // TODO: accommodate different arrangements of sent data (ONLY default supported now)
    int16_t qI[4];
    uint8_t status = MPU6050_dmpGetQuaternion16(qI, packet);
    if (status == 0) {
        q -> w = (float)qI[0] / 16384.0f;
        q -> x = (float)qI[1] / 16384.0f;
        q -> y = (float)qI[2] / 16384.0f;
        q -> z = (float)qI[3] / 16384.0f;
    }
}
void MPU6050_dmpGetAccel(struct VectorInt16_Base *v, const uint8_t* packet) {
    // TODO: accommodate different arrangements of sent data (ONLY default supported now)
    if (packet == 0) packet = MPUdmpPacketBuffer;
    v -> x = (packet[28] << 8) | packet[29];
    v -> y = (packet[32] << 8) | packet[33];
    v -> z = (packet[36] << 8) | packet[37];
}
void MPU6050_dmpGetGravity(struct VectorFloat_Base *v,struct Quaternion_Base *q) {
    v -> x = 2 * (q -> x*q -> z - q -> w*q -> y);
    v -> y = 2 * (q -> w*q -> x + q -> y*q -> z);
    v -> z = q -> w*q -> w - q -> x*q -> x - q -> y*q -> y + q -> z*q -> z;
}
void MPU6050_dmpGetLinearAccel(struct VectorInt16_Base *v,struct VectorInt16_Base *vRaw,struct VectorFloat_Base *gravity) {
    // get rid of the gravity component (+1g = +8192 in standard DMP FIFO packet, sensitivity is 2g)
    v -> x = vRaw -> x - gravity -> x*8192;
    v -> y = vRaw -> y - gravity -> y*8192;
    v -> z = vRaw -> z - gravity -> z*8192;
}
void MPU6050_dmpGetYawPitchRoll(float *data,struct Quaternion_Base *q,struct VectorFloat_Base *gravity) {
    // yaw: (about Z axis)
  data[0] = atan2((2*q -> x*q -> y - 2*q -> w*q -> z), (2*q -> w*q -> w + 2*q -> x*q -> x - 1));
    // pitch: (nose up/down, about Y axis)
    data[1] = atan2(gravity -> x , (sqrt(gravity -> y*gravity -> y + gravity -> z*gravity -> z)));
    // roll: (tilt left/right, about X axis)
    data[2] = atan2(gravity -> y , gravity -> z);
    if (gravity->z < 0) {
        if(data[1] > 0) {
            data[1] = M_PI - data[1];
        } else {
            data[1] = -M_PI - data[1];
        }
    }
}
void MPU6050_PID(uint8_t ReadAddress, float kP,float kI, uint8_t Loops)
{
  uint8_t SaveAddress = (ReadAddress == 0x3B)?((MPU6050_getDeviceID() < 0x38 )? 0x06:0x77):0x13;

    int16_t  Data;
    uint8_t data[1];
    float Reading;
    int16_t BitZero[3];
    uint8_t shift =(SaveAddress == 0x77)?3:2;
    float Error, PTerm, ITerm[3];
    int16_t eSample;
    uint32_t eSum;
    uint16_t gravity = 8192; // prevent uninitialized compiler warning
    if (ReadAddress == 0x3B)

      {
        gravity = 16384 >> MPU6050_getFullScaleAccelRange();
      }
    for (int i = 0; i < 3; i++) {
      i2c_read(MPU6050_ADD, SaveAddress + (i * shift), data,2); // reads 1 or more 16 bit integers (Word)
      Data = (int16_t)(data[0] <<8 | data[1]);
      Reading = Data;
      if(SaveAddress != 0x13){
        BitZero[i] = Data & 1;                     // Capture Bit Zero to properly handle Accelerometer calibration
        ITerm[i] = ((float)Reading) * 8;
        } else {
        ITerm[i] = Reading * 4;
      }
    }
    for (int L = 0; L < Loops; L++) {
      eSample = 0;
      for (int c = 0; c < 100; c++) {// 100 PI Calculations
        eSum = 0;
        for (int i = 0; i < 3; i++) {
          i2c_read(MPU6050_ADD, ReadAddress + (i * 2), data,2); // reads 1 or more 16 bit integers (Word)
          Data = (data[0]<<8 |data[1]);
          Reading = Data;
          if ((ReadAddress == 0x3B)&&(i == 2)) {Reading -= gravity;}  //remove Gravity
          Error = -Reading;
          eSum += abs(Reading);
          PTerm = kP * Error;
          ITerm[i] += (Error * 0.001) * kI;       // Integral term 1000 Calculations a second = 0.001
          if(SaveAddress != 0x13){
            Data = round((PTerm + ITerm[i] ) / 8);    //Compute PID Output
            Data = ((Data)&0xFFFE) |BitZero[i];     // Insert Bit0 Saved at beginning
          } else Data = round((PTerm + ITerm[i] ) / 4); //Compute PID Output
          data[0] = (Data >> 8) & 0xFF;
          data[1] = Data & 0xFF;
          i2c_write_nBytes(MPU6050_ADD, SaveAddress + (i * shift), data, 2);
        }
        if((c == 99) && eSum > 1000){           // Error is still to great to continue
          c = 0;
        }
        if((eSum * ((ReadAddress == 0x3B)?.05: 1)) < 5) eSample++;  // Successfully found offsets prepare to  advance
        if((eSum < 100) && (c > 10) && (eSample >= 10)) break;    // Advance to next Loop
      }
      kP *= .75;
      kI *= .75;
      for (int i = 0; i < 3; i++){
        if(SaveAddress != 0x13) {
          Data = round((ITerm[i] ) / 8);    //Compute PID Output
          Data = ((Data)&0xFFFE) |BitZero[i]; // Insert Bit0 Saved at beginning
        } else Data = round((ITerm[i]) / 4);
        data[0] = (Data >> 8) & 0xFF;
        data[1] = Data & 0xFF;
        i2c_write_nBytes(MPU6050_ADD, SaveAddress + (i * shift), data, 2);
      }
    }
    MPU6050_resetFIFO();
    MPU6050_resetDMP();
}
void MPU6050_CalibrateAccel(uint8_t Loops) {

  float kP = 0.3;
  float kI = 20;
  float x;
  x = 1.05;
  kP *= x;
  kI *= x;
  MPU6050_PID( 0x3B, kP, kI,  Loops);
  app_log("check ok1\n");
}
void MPU6050_CalibrateGyro(uint8_t Loops) {
  double kP = 0.3;
  double kI = 90;
  float x;
  x = 1.05;
  kP *= x;
  kI *= x;
  MPU6050_PID( 0x43,  kP, kI,  Loops);
  app_log("check ok2\n");
}
void MPU6050_ConfigDMP(struct MPU6050_Base *mpu,uint8_t *devStatus,bool *dmpReady,uint8_t *mpuIntStatus,uint16_t *packetSize)
{
      mpu->dmpPacketSize = 42;
      app_log("Initializing I2C devices...\n");
      MPU6050_init_DMP();
      app_log("Initializing DMP...\n");
      *devStatus = DMP_Init();
      if(*devStatus == 0)
        {
          app_log("successful\n");
          MPU6050_CalibrateAccel(6);
          MPU6050_CalibrateGyro(6);
          MPU6050_setDMPEnabled(true);
          *mpuIntStatus = MPU6050_getIntStatus();
          *dmpReady = true;
          *packetSize = MPU6050_dmpGetFIFOPacketSize(&(*mpu));
        }

      else {
              // ERROR!
              // 1 = initial memory load failed
              // 2 = DMP configuration updates failed
              // (if it's going to break, usually the code will be 1)
              app_log("DMP Initialization failed (code ");
              app_log("%d",*devStatus);
              app_log(")\n");
          }
}
void MPU6050_GetData(struct MPU6050_Base *mpu,bool *dmpReady,volatile bool *mpuInterrupt,uint16_t *packetSize,uint8_t *mpuIntStatus)
{
  struct Quaternion_Base q;           // [w, x, y, z]         quaternion container
  struct VectorInt16_Base aa;         // [x, y, z]            accel sensor measurements
  struct VectorInt16_Base aaReal;     // [x, y, z]            gravity-free accel sensor measurements
  struct VectorInt16_Base aaWorld;    // [x, y, z]            world-frame accel sensor measurements
  struct VectorFloat_Base gravity;
  double ax;
  double ay;
  double az;
  double SVM;
  uint16_t fifoCount;     // count of all bytes currently in FIFO
  uint8_t fifoBuffer[64];
  float euler[3];         // [psi, theta, phi]    Euler angle container
  float ypr[3];
  if (!dmpReady)
      return;
  // wait for MPU interrupt or extra packet(s) available
    while (!*mpuInterrupt && fifoCount < *packetSize)
      {
        break;
      }
  // reset interrupt flag and get INT_STATUS byte
    *mpuInterrupt = false;
    *mpuIntStatus = MPU6050_getIntStatus ();
  // get current FIFO count
    fifoCount = MPU6050_getFIFOCount ();
  // check for overflow (this should never happen unless our code is too inefficient)
    if ((*mpuIntStatus & 0x10) || fifoCount == 1024)
      {
  // reset so we can continue cleanly
        MPU6050_resetFIFO ();
        app_log("FIFO overflow!\n");
  // otherwise, check for DMP data ready interrupt (this should happen frequently)
      }
    else if (*mpuIntStatus & 0x02)
      {
  // wait for correct available data length, should be a VERY short wait
        while (fifoCount < *packetSize) fifoCount = MPU6050_getFIFOCount ();
  // read a packet from FIFO
        MPU6050_getFIFOBytes (fifoBuffer, *packetSize);
        MPU6050_resetFIFO ();

  // track FIFO count here in case there is > 1 packet available
  // (this lets us immediately read more without waiting for an interrupt)
        fifoCount -= *packetSize;
  // display real acceleration, adjusted to remove gravity
        MPU6050_dmpGetQuaternion (&q, fifoBuffer);
        MPU6050_dmpGetAccel (&aa, fifoBuffer);
        MPU6050_dmpGetGravity (&gravity, &q);
        MPU6050_dmpGetLinearAccel (&aaReal, &aa, &gravity);
        MPU6050_dmpGetYawPitchRoll (ypr, &q, &gravity);
        ax = (aaReal.x) / 16384.0;
        ay = (aaReal.y) / 16384.0;
        az = (aaReal.z) / 16384.0;
        SVM = sqrt (pow(ax,2) + pow(ay,2) + pow(az,2)) + 1.0;
//        app_log("\t");
//        app_log("%d", (uint16_t )(SVM * 1000));
//        app_log("\t\n");
        app_log("------\n");
        app_log("%d    %d    %d\n",(uint16_t )(gravity.x * 1000),(uint16_t )(gravity.y * 1000),(uint16_t )(gravity.z * 1000));
//        while (SVM > 1.7 && (ypr[2] * 180/M_PI)< 10 )
//        {
//            set_LED('r');
//        }
      }

}
