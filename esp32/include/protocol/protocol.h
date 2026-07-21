#pragma once
#include <stdint.h>

#define FRAME_START     0xAA
#define FRAME_END       0x55

#define MSG_SENSOR_DATA 0x01
#define MSG_ACK         0x02
#define MSG_COMMAND     0x03
#define MSG_ERROR       0x04

#define ACK_OK          0x00
#define ACK_CRC_ERROR   0x01
#define ACK_ERROR       0x02