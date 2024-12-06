/**
 ******************************************************************************
 * @file           : helperFunctions.c
 * @author         : Mostafa Asaad (https://github.com/M0stafa077)
 * @brief          : Source code for some helper functions
 ******************************************************************************
 */
#include "helperFunctions/helperFunctions.h"

uint32_t convertWordToBigEndian(uint32_t word) {
    uint32_t reversedWord  = 0;
	reversedWord |= (word & 0xFF) << 24;
    reversedWord |= ((word >> 8) & 0xFF) << 16;
    reversedWord |= ((word >> 16) & 0xFF) << 8;
    reversedWord |= ((word >> 24) & 0xFF);
    return reversedWord;
}

void turnLedOn(void) {
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);
}
void turnLedOff(void) {
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
}
HAL_StatusTypeDef sendToHost(uint8_t * message, uint8_t length) {
	return HAL_UART_Transmit(BOOTLOADER_UART_OBJECT, message, length, HAL_MAX_DELAY);
}

HAL_StatusTypeDef receiveFromHost(uint8_t * buffer, uint8_t length) {
	return HAL_UART_Receive(BOOTLOADER_UART_OBJECT, buffer, length, HAL_MAX_DELAY);
}

void sendDebuggingMessage(uint8_t * message, uint8_t length) {
	HAL_UART_Transmit(BOOTLOADER_UART_OBJECT, message, length, HAL_MAX_DELAY);
}

