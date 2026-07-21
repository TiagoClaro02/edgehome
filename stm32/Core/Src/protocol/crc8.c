/*
 * crc8.c
 *
 *  Created on: 21/07/2026
 *      Author: tmcla
 */


#include "protocol/crc8.h"

uint8_t crc8(const uint8_t *data, size_t len)
{
    uint8_t crc = 0x00;
    for (size_t i = 0; i < len; i++)
    {
        crc ^= data[i];
        for (uint8_t j = 0; j < 8; j++)
        {
            if (crc & 0x80)
                crc = (crc << 1) ^ 0x07;  // polynomial 0x07 (CRC-8-CCITT)
            else
                crc <<= 1;
        }
    }
    return crc;
}
