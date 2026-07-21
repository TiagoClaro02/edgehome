/*
 * crc8.h
 *
 *  Created on: 21/07/2026
 *      Author: tmcla
 */

#ifndef INC_PROTOCOL_CRC8_H_
#define INC_PROTOCOL_CRC8_H_

#pragma once
#include <stdint.h>
#include <stddef.h>

uint8_t crc8(const uint8_t *data, size_t len);


#endif /* INC_PROTOCOL_CRC8_H_ */
