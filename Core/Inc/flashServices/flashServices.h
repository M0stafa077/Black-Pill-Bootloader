/**
 ******************************************************************************
 * @file           : flashServices.h
 * @author         : Mostafa Asaad (https://github.com/M0stafa077)
 * @brief          : Flash memory services interface
 ******************************************************************************
 */

#ifndef INC_FLASHSERVICES_FLASHSERVICES_H_
#define INC_FLASHSERVICES_FLASHSERVICES_H_

/*---------------  Section: Includes --------------- */

#include "stm32f4xx_hal.h"
#include "Bootloader/Bootloader_Cfg.h"
#include "helperFunctions/helperFunctions.h"

/* --------------- Section: Macros Declarations --------------- */
#define E_OK    						0x0
#define E_NOT_OK    					0x1

typedef uint8_t Std_ReturnType_t;

/*---------------  Section: Functions Declaration --------------- */

Std_ReturnType_t Flash_Erase_Mass(void);
HAL_StatusTypeDef flashWrite(uint32_t address, uint8_t* data, uint32_t length) ;

#endif /* INC_FLASHSERVICES_FLASHSERVICES_H_ */
