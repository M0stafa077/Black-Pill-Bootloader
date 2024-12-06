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

static uint8_t receivedBuffer[BOOTLOADER_MAX_BUFFER_SIZE];

/*---------------  Section: Static Functions Declaration --------------- */
static BL_ReturnType_t Bootloader_Get_Version(void);
static BL_ReturnType_t Bootloader_Get_Help(void);
static BL_ReturnType_t Bootloader_Get_Chip_ID(void);
static BL_ReturnType_t Bootloader_Get_RDP_Status(void);
static BL_ReturnType_t Bootloader_Jump_To_User_App();
static BL_ReturnType_t Bootloader_GoTo_Address(void);
static BL_ReturnType_t Bootloader_EraseFlash(void);
static BL_ReturnType_t Bootloader_writeFlashMemory(void);
static BL_ReturnType_t Bootloader_readFromFlash(void);

static BL_ReturnType_t BL_Send_ACK_Message(uint8_t Reply_Lenght);
static BL_ReturnType_t BL_Send_NACK_Message();
static CRC_State_t BL_Check_CRC_Matching();
static inline uint8_t BL_IsValidAddress(uint32_t userAddress);
uint32_t calculateCRC32(const uint8_t* buffer, uint8_t bufferLength);
/*---------------  Section: Function Definitions --------------- */

BL_ReturnType_t Bootloader_Fetch_Host_Command(void)
{
	BL_ReturnType_t bootloaderStatus = BL_OK;
	HAL_StatusTypeDef HAL_State = HAL_OK;
	uint8_t packetLength = 0;


	/* Reset the Buffer */
	memset(receivedBuffer, BOOTLOADER_BUFFER_RESET, BOOTLOADER_MAX_BUFFER_SIZE);

	/* Receive the first byte => Data Length [N] */
	HAL_State = receiveFromHost(receivedBuffer, 1);

	if(HAL_State == HAL_OK)
	{
		packetLength = *receivedBuffer;

		/* Receive the Second Byte => The Command */
		HAL_State = receiveFromHost(&receivedBuffer[1], packetLength);


		if(HAL_State == HAL_OK) {
			switch(receivedBuffer[1]) {
				case CBL_GET_VER_CMD:
					bootloaderStatus |= Bootloader_Get_Version();
					break;
				case CBL_GET_HELP_CMD:
					bootloaderStatus |= Bootloader_Get_Help();
					break;
				case CBL_GET_CID_CMD:
					/* Chip ID = 0x423 */
					bootloaderStatus |= Bootloader_Get_Chip_ID();
					break;
				case CBL_GET_RDP_STATUS_CMD:
					bootloaderStatus |= Bootloader_Get_RDP_Status();
					break;
				case CBL_GO_TO_ADDR_CMD:
					/* Go to address Function */
					bootloaderStatus |= Bootloader_GoTo_Address();
					break;
				case CBL_GOTO_USER_APP_CMD:
					bootloaderStatus |= Bootloader_Jump_To_User_App();
					break;
				case CBL_FLASH_ERASE_CMD:
					/* Erase Flash Function */
					bootloaderStatus |= Bootloader_EraseFlash();
					break;
				case CBL_MEM_WRITE_CMD:
					/* Mmemory Write Function */
					bootloaderStatus |= Bootloader_writeFlashMemory();
					break;
				case CBL_MEM_READ_CMD:
					/* Memory Read Function */
					bootloaderStatus |= Bootloader_readFromFlash();
					break;
				default: bootloaderStatus |= BL_NOT_OK;
					break;
			}
		}
		else {
			bootloaderStatus |= BL_NOT_OK;	/* Reception is not successfull */
		}

	}
	else {
		bootloaderStatus |= BL_NOT_OK;	/* Reception is not successfull */
	}
	return bootloaderStatus;
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

	/* Transmit the Version message */
	if(Bootloader_State == BL_OK)
	{
		UART_State = sendToHost(reply_message, (uint8_t)sizeof(reply_message));
	}

	if(UART_State == HAL_OK)
		{ Bootloader_State |= BL_OK; }
	else
		{ Bootloader_State |= BL_NOT_OK; }

	return Bootloader_State;
}

static BL_ReturnType_t Bootloader_Get_Help(void)
{
	uint8_t reply_message[] = {
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
	if(CRC_State == CRC_NOT_MATCH) {
		return BL_NOT_OK;
	}
	/* Send ACK message */
	Bootloader_State |= BL_Send_ACK_Message(sizeof(reply_message));
	HAL_Delay(5);

	/* Transmit the Help message */
	if(Bootloader_State == BL_OK) {
		UART_State = sendToHost(reply_message, (uint8_t)sizeof(reply_message));
	}

	if(UART_State == HAL_OK)
		{ Bootloader_State |= BL_OK; }
	else
		{ Bootloader_State |= BL_NOT_OK; }

	return Bootloader_State;
}

static BL_ReturnType_t Bootloader_Get_Chip_ID(void) {
	CRC_State_t CRC_State = CRC_MATCH;
	BL_ReturnType_t Bootloader_State = BL_OK;
	HAL_StatusTypeDef UART_State = HAL_OK;
	uint16_t reply_message = 0;

	/* Get the reply messgage */
	reply_message = DBGMCU->IDCODE & 0xFFF;

	/* Calculate the CRC For the received messgae */

	CRC_State = BL_Check_CRC_Matching();
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
		UART_State = sendToHost(&Low_Byte, 1);
		UART_State = sendToHost(&High_Byte, 1);
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
	CRC_State = BL_Check_CRC_Matching();
	if(CRC_State == CRC_NOT_MATCH)
	{
		return BL_NOT_OK;	/* CRC Error */
	}

	/* Send ACK message */
	Bootloader_State |= BL_Send_ACK_Message(1);

	/* Transmit the RDP Level */
	if(Bootloader_State == BL_OK)
	{
		UART_State = sendToHost(&reply_message, sizeof(reply_message));
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
	/* Get the Reset Handler function of the user application */
	newAppResetHandlerAddress = *((volatile uint32_t *)(FLASH_SECTOR_2_BASE_ADD + 4));
#elif USER_APPLICATION_SECTOR == FLASH_SECTOR_4
	MSP_Value = *((volatile uint32_t *)(FLASH_SECTOR_4_BASE_ADD));
	/* Get the Reset Handler function of the user application */
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

/**
 * CRC_MATCH => send ack.
 *
 */
static BL_ReturnType_t Bootloader_GoTo_Address(void)
{
	uint32_t userAddress = *((uint32_t *)(&receivedBuffer[2]));
	uint8_t isValidAddress = BL_IsValidAddress(userAddress);
	CRC_State_t CRC_State = CRC_MATCH;

	CRC_State = BL_Check_CRC_Matching();
	if(CRC_State == CRC_MATCH) {
		BL_Send_ACK_Message(0);
	}
	else {
		return BL_NOT_OK;
	}

	if(isValidAddress) {
		((pToFun)(userAddress | 0x01UL))();
	} else {
		return BL_NOT_OK;
	}
	return BL_OK;
}

static BL_ReturnType_t Bootloader_EraseFlash(void) {
	CRC_State_t CRCState = CRC_MATCH;
	CRCState = BL_Check_CRC_Matching();
	if(CRCState == CRC_MATCH) {
		BL_Send_ACK_Message(0);
	}
	else {
		return BL_NOT_OK;
	}
	return Flash_Erase_Mass();
}

static BL_ReturnType_t Bootloader_writeFlashMemory(void) {
	CRC_State_t CRCState = CRC_MATCH;
	uint32_t baseAddress = *((uint32_t *)(&receivedBuffer[2]));
	uint8_t dataLength = receivedBuffer[0] - 10;
	BL_ReturnType_t bootloaderStatus = BL_OK;

	CRCState = BL_Check_CRC_Matching();
	if(CRCState == CRC_MATCH) {
		BL_Send_ACK_Message(1);
	}
	else {
		BL_Send_NACK_Message();
	}

    // Reverse the byte order
    baseAddress = convertWordToBigEndian(baseAddress);

	uint8_t isValidAddress = ((baseAddress >= FLASH_BASE) && (baseAddress <= FLASH_END));
	if (!isValidAddress) {
		sendToHost((uint8_t *) "X", 1);
		return BL_NOT_OK;
	}

	bootloaderStatus |= flashWrite(baseAddress, (uint8_t *)&receivedBuffer[6], dataLength);

    if(bootloaderStatus) {
    	sendToHost((uint8_t *) "E", 1);
    } else {
    	sendToHost((uint8_t *) "O", 1);
    }
	return bootloaderStatus;
}

static BL_ReturnType_t Bootloader_readFromFlash(void) {
	CRC_State_t CRCState = CRC_MATCH;
	uint32_t baseAddress = *((uint32_t *)(&receivedBuffer[2]));
	uint8_t dataLength = receivedBuffer[6];
	BL_ReturnType_t bootloaderStatus = BL_OK;

	CRCState = BL_Check_CRC_Matching();
	if(CRCState == CRC_MATCH) {
		BL_Send_ACK_Message(dataLength);
	}
	else {
		BL_Send_NACK_Message();
	}
    // Reverse the byte order
    baseAddress = convertWordToBigEndian(baseAddress);
	uint8_t isValidAddress = ((baseAddress >= FLASH_BASE) && (baseAddress <= FLASH_END)) && (baseAddress % 4 == 0);
	if (!isValidAddress) {
		sendToHost((uint8_t *) "E", 1);
		sendDebuggingMessage((uint8_t *)"Invalid Address", 3);
		return BL_NOT_OK;
	}

	for(uint8_t i = 0; i < dataLength; ++i) {
		uint32_t data = *((volatile uint32_t *)baseAddress);

		for(uint8_t j = 0; j < 4; ++j) {
			bootloaderStatus |= sendToHost((uint8_t *)&data, 1);
		}
	}
	return bootloaderStatus;
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

static BL_ReturnType_t BL_Send_NACK_Message()
{
	HAL_StatusTypeDef UART_State = HAL_OK;
	uint8_t acknowledge_message[1] = { BL_NACK_MESSAGE };

	/* Transmit the acknowledge message over UART */
	UART_State = HAL_UART_Transmit(BOOTLOADER_UART_OBJECT, acknowledge_message,
			1, HAL_MAX_DELAY);

	return (UART_State == HAL_OK) ? BL_OK : BL_NOT_OK;
}
uint32_t calculateCRC32(const uint8_t* buffer, uint8_t bufferLength) {
  uint32_t CRC_Value = 0xFFFFFFFF;

  for (uint8_t i = 0; i < bufferLength; ++i) {
    CRC_Value ^= buffer[i];

    for (int DataElemBitLen = 0; DataElemBitLen < 32; ++DataElemBitLen) {  // 8 bits per byte
      if (CRC_Value & 0x80000000) {
        CRC_Value = (CRC_Value << 1) ^ 0x04C11DB7;
      } else {
        CRC_Value = (CRC_Value << 1);
      }
    }

  }
  return convertWordToBigEndian((uint32_t)(CRC_Value & 0xFFFFFFFF));
}
static CRC_State_t BL_Check_CRC_Matching(void)
{
	uint32_t crcResult = 0xFFFFFFFF;
	uint32_t hostCRC = 0;
	uint8_t packetLen = receivedBuffer[0] + 1;
	/* Get the received CRC */
	hostCRC = *((uint32_t *)((receivedBuffer + packetLen) - 4));
	crcResult = calculateCRC32(receivedBuffer, packetLen - 4);
	return (hostCRC == crcResult) ? CRC_MATCH : CRC_NOT_MATCH;
}

static inline uint8_t BL_IsValidAddress(uint32_t userAddress)
{
	//	Address is valid only if it's within the SRAM or the FLASH memories
	return (((userAddress >= SRAM1_BASE) && (userAddress <= 0x2000FFFF))
			|| ((userAddress >= FLASH_BASE) && (userAddress <= FLASH_END)));
}
