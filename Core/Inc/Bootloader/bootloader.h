/**
 ******************************************************************************
 * @file           : bootloader.h
 * @author         : Mostafa Asaad (https://github.com/M0stafa077)
 * @brief          : Bootloader header file interface.
 ******************************************************************************
 */


#ifndef INC_BOOTLOADER_BOOTLOADER_H_
#define INC_BOOTLOADER_BOOTLOADER_H_

/* --------------- Section : Includes --------------- */
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_flash_ex.h"
#include "Bootloader_Cfg.h"
#include "flashServices/flashServices.h"
#include "helperFunctions/helperFunctions.h"
/* --------------- Section: Macro Declarations --------------- */

/* !< Bootloader Supported Commands */

#define CBL_GET_VER_CMD             0x10
#define CBL_GET_HELP_CMD            0x11
#define CBL_GET_CID_CMD             0x12
/* Get Read Protection Status */
#define CBL_GET_RDP_STATUS_CMD      0x13
#define CBL_GOTO_USER_APP_CMD		0x22
#define CBL_GO_TO_ADDR_CMD          0x14
#define CBL_FLASH_ERASE_CMD         0x15
#define CBL_MEM_WRITE_CMD           0x16
/* Enable/Disable Write Protection */
#define CBL_ED_W_PROTECT_CMD        0x17
#define CBL_MEM_READ_CMD            0x18
/* Get Sector Read/Write Protection Status */
#define CBL_READ_SECTOR_STATUS_CMD  0x19
#define CBL_OTP_READ_CMD            0x20
/* Change Read Out Protection Level */
#define CBL_CHANGE_ROP_Level_CMD    0x21

/* !< The UART Module Configurations Object */
#define BOOTLOADER_UART_OBJECT		&huart2

/* !< The CRC Module Configurations Object */
#define BOOTLOADER_CRC_OBJECT		&hcrc

/* !< The Maximum received buffer size in bytes */
#define BOOTLOADER_MAX_BUFFER_SIZE	100

/* !< The Reset value for the received buffer */
#define BOOTLOADER_BUFFER_RESET		0

/* !< Bootlaoder version details */
#define BL_VENDOR_ID				0x22
#define BL_MAJOR_VERSION			0x01
#define BL_MINOR_VERSION			0x00
#define BL_PATCH_VERSION			0x00

/* !< Bootloader ACK message */
#define BL_ACK_MESSAGE				0xDD
/* !< Bootloader NACK message */
#define BL_NACK_MESSAGE				0xEE

#define FLASH_SECTOR_2_BASE_ADD		0x08008000
#define FLASH_SECTOR_4_BASE_ADD		0x08010000

#define BL_VALID_ADDRESS			(uint8_t)0x01
#define BL_INVALID_ADDRESS			(uint8_t)0x00
/* --------------- Section: External Variables --------------- */
extern CRC_HandleTypeDef hcrc;
extern UART_HandleTypeDef huart2;
/* --------------- Section: Macro Functions Declarations --------------- */

/* --------------- Section: Data Type Declarations --------------- */

typedef enum
{
	BL_OK = 0,
	BL_NOT_OK = 1
} BL_ReturnType_t;

typedef enum
{
	CRC_MATCH,
	CRC_NOT_MATCH
} CRC_State_t;

typedef void (* pToFun) (void);

/*---------------  Section: Function Declarations --------------- */

BL_ReturnType_t Bootloader_Fetch_Host_Command(void);

#endif /* INC_BOOTLOADER_BOOTLOADER_H_ */
