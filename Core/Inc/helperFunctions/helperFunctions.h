/**
 ******************************************************************************
 * @file           : helperFunctions.h
 * @author         : Mostafa Asaad (https://github.com/M0stafa077)
 * @brief          : Interface for some helper functions
 ******************************************************************************
 */

#ifndef INC_HELPERFUNCTIONS_HELPERFUNCTIONS_H_
#define INC_HELPERFUNCTIONS_HELPERFUNCTIONS_H_

#include <stdint.h>
#include "stm32f4xx_hal.h"

/* --------------- Section: External Variables --------------- */
extern UART_HandleTypeDef huart2;

#define BOOTLOADER_UART_OBJECT  	&huart2

/* --------------- Section: Functions Declaration --------------- */

uint32_t convertWordToBigEndian(uint32_t word);

void turnLedOn(void);

void turnLedOff(void);

HAL_StatusTypeDef sendToHost(uint8_t * message, uint8_t length);

HAL_StatusTypeDef receiveFromHost(uint8_t * buffer, uint8_t length);

void sendDebuggingMessage(uint8_t * message, uint8_t length);

#endif /* INC_HELPERFUNCTIONS_HELPERFUNCTIONS_H_ */
