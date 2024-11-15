/**
 ******************************************************************************
 * @file           : bootloader.c
 * @author         : Mostafa Asaad (https://github.com/M0stafa077)
 * @brief          : Bootloader static code implementation
 ******************************************************************************
 */

/* --------------- Section : Includes --------------- */
#include "Bootloader/bootloader.h"
/*---------------  Section: Global Variables --------------- */
static uint8_t Received_Buffer[BOOTLOADER_MAX_BUFFER_SIZE];

/*---------------  Section: Static Functions Declaration --------------- */
static BL_ReturnType_t Bootloader_Get_Version(void);
static BL_ReturnType_t Bootloader_Get_Help(void);
static BL_ReturnType_t Bootloader_Get_Chip_ID(void);
static BL_ReturnType_t Bootloader_Get_RDP_Status(void);
static BL_ReturnType_t Bootloader_Jump_To_User_App();
static BL_ReturnType_t Bootloader_GoTo_Address(void);

static BL_ReturnType_t BL_Send_ACK_Message(uint8_t Reply_Lenght);
static CRC_State_t BL_Check_CRC_Matching();
static uint8_t BL_IsValidAddress(uint32_t userAddress);
/*---------------  Section: Function Definitions --------------- */

BL_ReturnType_t Bootloader_Fetch_Host_Command(void)
{
	BL_ReturnType_t Bootloader_State = BL_OK;
	HAL_StatusTypeDef HAL_State = HAL_OK;
	uint8_t Packet_Length = 0;
	/* Reset the Buffer */
	memset(Received_Buffer, BOOTLOADER_BUFFER_RESET, BOOTLOADER_MAX_BUFFER_SIZE);

	/* Receive the first byte => Data Length [N] */
	HAL_State = HAL_UART_Receive(BOOTLOADER_UART_OBJECT, Received_Buffer,
							1, HAL_MAX_DELAY);

	if(!HAL_State)	/* HAL_OK */
	{
		Packet_Length = Received_Buffer[0];

		/* Receive the Second Byte => The Command */
		HAL_State = HAL_UART_Receive(BOOTLOADER_UART_OBJECT, Received_Buffer + 1,
							Packet_Length, HAL_MAX_DELAY);
		if(!HAL_State) /* if HAL_OK */
		{
			switch(Received_Buffer[1])
			{
				case CBL_GET_VER_CMD:
					Bootloader_State |= Bootloader_Get_Version();
					break;
				case CBL_GET_HELP_CMD:
					Bootloader_State |= Bootloader_Get_Help();
					break;
				case CBL_GET_CID_CMD:
					/* Chip ID = 0x423 */
					Bootloader_State |= Bootloader_Get_Chip_ID();
					break;
				case CBL_GET_RDP_STATUS_CMD:
					Bootloader_State |= Bootloader_Get_RDP_Status();
					break;
				case CBL_GO_TO_ADDR_CMD:
					/* Go to address Function */
					break;
				case CBL_GOTO_USER_APP_CMD:
					Bootloader_State |= Bootloader_Jump_To_User_App();
					break;
				case CBL_FLASH_ERASE_CMD:
					/* Erase Flash Function */
					break;
				case CBL_MEM_WRITE_CMD:
					/* Mmemory Write Function */
					break;
				case CBL_ED_W_PROTECT_CMD:
					/* Function */
					break;
				case CBL_MEM_READ_CMD:
					/* Memory Read Function */
					break;
				case CBL_READ_SECTOR_STATUS_CMD:
					/* Read Sector Status Function */
					break;
				case CBL_OTP_READ_CMD:
					/* Read OTP Function */
					break;
				case CBL_CHANGE_ROP_Level_CMD:
					/* Change Read Out Protection Function */
					break;
				default: Bootloader_State |= BL_NOT_OK;
					break;
			}
		}
		else
		{
			Bootloader_State |= BL_NOT_OK;	/* Reception is not successfull */
		}

	}
	else
	{
		Bootloader_State |= BL_NOT_OK;	/* Reception is not successfull */
	}
//	Bootloader_Jump_To_User_App();
	return Bootloader_State;
}

/*---------------  Section: Static Functions Implementation --------------- */
static BL_ReturnType_t Bootloader_Get_Version(void)
{
	uint8_t reply_message[] = { BL_VENDOR_ID, BL_MAJOR_VERSION, BL_MINOR_VERSION,
								BL_PATCH_VERSION };
	BL_ReturnType_t Bootloader_State = BL_OK;
	CRC_State_t CRC_State = CRC_MATCH;
	HAL_StatusTypeDef UART_State = HAL_OK;

	/* Calculate the CRC For the received messgae */
	CRC_State = BL_Check_CRC_Matching();
	if(CRC_State == CRC_NOT_MATCH)
	{
		return BL_NOT_OK;
	}
	/* Send ACK message */
	Bootloader_State |= BL_Send_ACK_Message(sizeof(reply_message));
	HAL_Delay(5);

	/* Transmit the Version message */
	if(Bootloader_State == BL_OK)
	{
		UART_State = HAL_UART_Transmit(BOOTLOADER_UART_OBJECT, reply_message,
				sizeof(reply_message), HAL_MAX_DELAY);
	}

	if(UART_State == HAL_OK)
		{ Bootloader_State |= BL_OK; }
	else
		{ Bootloader_State |= BL_NOT_OK; }

	return Bootloader_State;
}

static BL_ReturnType_t Bootloader_Get_Help(void)
{
	uint8_t reply_message[] =
	{
		CBL_GET_VER_CMD,
		CBL_GET_HELP_CMD,
		CBL_GET_CID_CMD,
		CBL_GET_RDP_STATUS_CMD,
		CBL_GO_TO_ADDR_CMD,
		CBL_FLASH_ERASE_CMD,
		CBL_MEM_WRITE_CMD,
		CBL_ED_W_PROTECT_CMD,
		CBL_MEM_READ_CMD,
		CBL_READ_SECTOR_STATUS_CMD,
		CBL_OTP_READ_CMD,
		CBL_CHANGE_ROP_Level_CMD
	};
	BL_ReturnType_t Bootloader_State = BL_OK;
	CRC_State_t CRC_State = CRC_MATCH;
	HAL_StatusTypeDef UART_State = HAL_OK;

	/* Calculate the CRC For the received messgae */
	CRC_State = BL_Check_CRC_Matching();
	if(CRC_State == CRC_NOT_MATCH)
	{
		return BL_NOT_OK;
	}
	/* Send ACK message */
	Bootloader_State |= BL_Send_ACK_Message(sizeof(reply_message));
	HAL_Delay(5);

	/* Transmit the Help message */
	if(Bootloader_State == BL_OK)
	{
		UART_State = HAL_UART_Transmit(BOOTLOADER_UART_OBJECT, reply_message,
				sizeof(reply_message), HAL_MAX_DELAY);
	}

	if(UART_State == HAL_OK)
		{ Bootloader_State |= BL_OK; }
	else
		{ Bootloader_State |= BL_NOT_OK; }

	return Bootloader_State;
}

static BL_ReturnType_t Bootloader_Get_Chip_ID(void)
{
	CRC_State_t CRC_State = CRC_MATCH;
	BL_ReturnType_t Bootloader_State = BL_OK;
	HAL_StatusTypeDef UART_State = HAL_OK;
	uint16_t reply_message = 0;

	/* Get the reply messgage */
	reply_message = DBGMCU->IDCODE & 0xFFF;

	/* Calculate the CRC For the received messgae */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreturn-type"
	CRC_State = BL_Check_CRC_Matching();
#pragma GCC diagnostic pop
	if(CRC_State == CRC_NOT_MATCH)
	{
		return BL_NOT_OK;
	}
	/* Send ACK message */
	Bootloader_State |= BL_Send_ACK_Message(sizeof(reply_message));
	HAL_Delay(5);

	/* Transmit the Chip ID */
	if(Bootloader_State == BL_OK)
	{
		uint8_t Low_Byte  = (uint8_t)(reply_message & 0xFF);
		uint8_t High_Byte = (uint8_t)(reply_message >> 8);
		UART_State = HAL_UART_Transmit(BOOTLOADER_UART_OBJECT, &Low_Byte,
									1, HAL_MAX_DELAY);
		HAL_Delay(2);
		UART_State = HAL_UART_Transmit(BOOTLOADER_UART_OBJECT, &High_Byte,
									1, HAL_MAX_DELAY);
	}
	if(UART_State == HAL_OK)
		{ Bootloader_State |= BL_OK; }
	else
		{ Bootloader_State |= BL_NOT_OK; }

	return Bootloader_State;
}

static BL_ReturnType_t Bootloader_Get_RDP_Status(void)
{
	BL_ReturnType_t Bootloader_State = BL_OK;
	CRC_State_t CRC_State = CRC_MATCH;
	HAL_StatusTypeDef UART_State = HAL_OK;
	uint8_t reply_message = 0;

	/* Get the reply messgage */
	FLASH_OBProgramInitTypeDef OB_Config = { 0 };
	HAL_FLASHEx_OBGetConfig(&OB_Config);
	reply_message = (uint8_t)OB_Config.RDPLevel;

	/* Calculate the CRC For the received message */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreturn-type"
	CRC_State = BL_Check_CRC_Matching();
#pragma GCC diagnostic pop
	if(CRC_State == CRC_NOT_MATCH)
	{
		return BL_NOT_OK;	/* CRC Error */
	}

	/* Send ACK message */
	Bootloader_State |= BL_Send_ACK_Message(1);

	/* Transmit the RDP Level */
	if(Bootloader_State == BL_OK)
	{
		UART_State = HAL_UART_Transmit(BOOTLOADER_UART_OBJECT, &reply_message,
									1, HAL_MAX_DELAY);
	}
	if(UART_State == HAL_OK)
		{ Bootloader_State |= BL_OK; }
	else
		{ Bootloader_State |= BL_NOT_OK; }

	return Bootloader_State;
}

static BL_ReturnType_t Bootloader_Jump_To_User_App(void)
{
	/* Get Main Stack Pointer */
	uint32_t MSP_Value = 0;
	uint32_t newAppResetHandlerAddress = 0;
	CRC_State_t CRC_State = CRC_MATCH;

	CRC_State = BL_Check_CRC_Matching();
	if(CRC_State == CRC_MATCH)
		{ BL_Send_ACK_Message(0); }
	else
		{ return BL_NOT_OK; }

#if USER_APPLICATION_SECTOR == FLASH_SECTOR_2
	MSP_Value = *((volatile uint32_t *)(FLASH_SECTOR_2_BASE_ADD));
#elif USER_APPLICATION_SECTOR == FLASH_SECTOR_4
	MSP_Value = *((volatile uint32_t *)(FLASH_SECTOR_4_BASE_ADD));
#endif

	/* Get the Reset Handler function of the user application */
#if USER_APPLICATION_SECTOR == FLASH_SECTOR_2
	newAppResetHandlerAddress = *((volatile uint32_t *)(FLASH_SECTOR_2_BASE_ADD + 4));
#elif USER_APPLICATION_SECTOR == FLASH_SECTOR_4
	newAppResetHandlerAddress = *((volatile uint32_t *)(FLASH_SECTOR_4_BASE_ADD + 4));
#endif
	pToFun newAppResetHandler = (pToFun)newAppResetHandlerAddress;

	/* De-Initialize the running peripherals */
	HAL_RCC_DeInit();
	HAL_UART_DeInit(BOOTLOADER_UART_OBJECT);
	HAL_CRC_DeInit(BOOTLOADER_CRC_OBJECT);

	/* Initialize the new MSP */
	__set_MSP(MSP_Value);

	/* Call the Reset Handler function of the new App */
	newAppResetHandler();

	return BL_OK;
}

static BL_ReturnType_t Bootloader_GoTo_Address(void)
{

}


/* --------------------------------------------------------------------- */
static BL_ReturnType_t BL_Send_ACK_Message(uint8_t Reply_Lenght)
{
	HAL_StatusTypeDef UART_State = HAL_OK;
	uint8_t acknowledge_message[2] = { BL_ACK_MESSAGE, Reply_Lenght };

	/* Transmit the acknowledge message over UART */
	UART_State = HAL_UART_Transmit(BOOTLOADER_UART_OBJECT, acknowledge_message,
			2, HAL_MAX_DELAY);

	return (UART_State == HAL_OK) ? BL_OK : BL_NOT_OK;
}

static CRC_State_t BL_Check_CRC_Matching(void)
{
	uint32_t calculatedCRC = 0;
	uint32_t dummyWord = 0;
	uint8_t packetLen = Received_Buffer[0] + 1;
	uint32_t hostCRC = 0;
	/* Get the received CRC */
	hostCRC = *((uint32_t *)((Received_Buffer + packetLen) - 4));
	for(uint8_t i = 0; i < (packetLen - 4); ++i)
	{
		dummyWord = (uint32_t)Received_Buffer[i];
		calculatedCRC = HAL_CRC_Accumulate(BOOTLOADER_CRC_OBJECT, &dummyWord, 1);
	}
	// Reset the CRC Calculation Unit
	__HAL_CRC_DR_RESET(BOOTLOADER_CRC_OBJECT);
	// Return the result
	return (hostCRC == calculatedCRC) ? CRC_MATCH : CRC_NOT_MATCH;
}

static uint8_t BL_IsValidAddress(uint32_t userAddress)
{

}


