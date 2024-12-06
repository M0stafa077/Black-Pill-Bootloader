/**
 ******************************************************************************
 * @file           : flashServices.c
 * @author         : Mostafa Asaad (https://github.com/M0stafa077)
 * @brief          : Flash memory services implementation
 ******************************************************************************
 */

/*---------------  Section: Includes --------------- */

#include "flashServices/flashServices.h"

/* --------------- Section: Private Macros Declarations --------------- */

#define FLASH_CR_KEY_1					(0x45670123UL)
#define FLASH_CR_KEY_2					(0xCDEF89ABUL)

#define FLASH_PARALLELISM_32			(0x00000002UL)
#define FLASH_PSIIZE_POS				(0x08UL)
/* --------------- Section: Private Macro Functions Declarations --------------- */

#define FLASH_WAIT_FOR_COMPLETION()		while(READ_BIT(FLASH->SR, 16))
#define FLASH_START_OPERATION()			(SET_BIT(FLASH->CR, 16))
#define FLASH_OB_START_OPERATION()		(SET_BIT(FLASH->OPTCR, 1))

/*---------------  Section: Private Types Declarations --------------- */

typedef enum
{
	FLASH_SECTOR_0_NUMBER = 16,
	FLASH_SECTOR_1_NUMBER = 17,
	FLASH_SECTOR_2_NUMBER = 18,
	FLASH_SECTOR_3_NUMBER = 19,
	FLASH_SECTOR_4_NUMBER = 20,
	FLASH_SECTOR_5_NUMBER = 21
} Flash_Sector_t;

/*---------------  Section: Private Helper Function Declarations --------------- */
static Std_ReturnType_t Flash_Unlock(void);
static Std_ReturnType_t Flash_Lock(void);
static Std_ReturnType_t Flash_Erase_Sector(const Flash_Sector_t Sector);

/*---------------  Section: Functions Definition --------------- */

/**
 * @brief  Erases the used sector for the user app of FLASH memory.
 * @retval Std_ReturnType_t: Status of the operation.
 *         - E_OK: Operation completed successfully
 *         - E_NOT_OK: Operation failed
 */
Std_ReturnType_t Flash_Erase_Mass(void)
{
#if USER_APPLICATION_SECTOR==FLASH_SECTOR_2
	return Flash_Erase_Sector(FLASH_SECTOR_2_NUMBER);
#elif USER_APPLICATION_SECTOR==FLASH_SECTOR_4
	return Flash_Erase_Sector(FLASH_SECTOR_4_NUMBER);
#endif
}

/**
 * @brief  Programs a word (32-bit) at a specified address in the FLASH memory.
 * @param  address: Address in FLASH memory where the data should be written.
 *         This parameter must be a multiple of 4 bytes (word-aligned).
 * @param  data: Data to be programmed at the specified address.
 * @retval Std_ReturnType_t: Status of the operation.
 *         - E_OK: Operation completed successfully
 *         - E_NOT_OK: Operation failed (e.g., address not aligned, programming error)
 */
/**
 * @brief Write data to a specific address in flash memory.
 * @param address The starting flash memory address to write to.
 * @param data The data to write.
 * @param length The length of the data array.
 * @return HAL_StatusTypeDef Status of the flash write operation.
 */
HAL_StatusTypeDef flashWrite(uint32_t address, uint8_t* data, uint32_t length) {
    HAL_StatusTypeDef status = HAL_OK;
    uint32_t word = 0x00;

    HAL_FLASH_Unlock();

    for (uint32_t i = 0; i < length; i += 4) {
        word = data[i] | (data[i + 1] << 8) | (data[i + 2] << 16) | (data[i + 3] << 24);

        word = convertWordToBigEndian(word);

        status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, address + i, word);

        if (status != HAL_OK) {
            break;
        }
    }

    HAL_FLASH_Lock();

    return status;
}

/*---------------  Section: Private Helper Function Definitions --------------- */

static Std_ReturnType_t Flash_Unlock(void) {
	// Disable interrupts
	__disable_irq();

	if(!READ_BIT(FLASH->CR, 31)) {
		// Control register is already unlocked
		return E_OK;
	}
	// Unlock the Control Register
	FLASH->KEYR = FLASH_CR_KEY_1;
	FLASH->KEYR = FLASH_CR_KEY_2;

	// Return the status
	return (READ_BIT(FLASH->CR, 31) ? E_NOT_OK : E_OK);
}

static Std_ReturnType_t Flash_Lock(void) {
	// Set the LOCK bit
	SET_BIT(FLASH->CR, 31);

	// Re-enable interrupts
    __enable_irq();

    // Return the status
	return (READ_BIT(FLASH->CR, 31) ? E_OK : E_NOT_OK);
}

/**
 * @brief  Erases a specific sector of FLASH memory.
 * @param  Sector: The sector to be erased.
 *         This parameter should be of type `Flash_Sector_t` and
 *         indicate the specific FLASH sector to erase.
 * @retval Std_ReturnType_t: Status of the operation.
 *         - E_OK: Operation completed successfully
 *         - E_NOT_OK: Operation failed
 */
static Std_ReturnType_t Flash_Erase_Sector(const Flash_Sector_t Sector)
{
	Std_ReturnType_t retVal = E_OK;

	if((uint32_t)Sector > (uint32_t)FLASH_SECTOR_5) {
		retVal |= E_NOT_OK;
	}
	else
	{
		/* 1. Wait for the Flash Memory to be free */
		FLASH_WAIT_FOR_COMPLETION();

		/* 2. Unlock the Control register */
		retVal |= Flash_Unlock();
		if(E_NOT_OK == retVal) {
			return retVal;
		}

		/* 3. Set the SER bit */
		FLASH->CR |= (1UL << 1);

		/* 4. Select the Sector to be erased */
		FLASH->CR |= (uint32_t) (((uint32_t)Sector & 0x0000000F) << 3);

		/* 5. Start the erase operation */
		FLASH_START_OPERATION();

		/* 6. Wait for the Flash to complete the operation */
		FLASH_WAIT_FOR_COMPLETION();

		/* 7. Lock the Control register */
		retVal |= Flash_Lock();
	}
	return retVal;
}
