/*******************************************************************************
  AT25DF Driver Local Data Structures

  Company:
    Microchip Technology Inc.

  File Name:
    drv_at25df_local.h

  Summary:
    AT25DF Driver Local Data Structures

  Description:
    Driver Local Data Structures
*******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
* Copyright (C) 2018 Microchip Technology Inc. and its subsidiaries.
*
* Subject to your compliance with these terms, you may use Microchip software
* and any derivatives exclusively with Microchip products. It is your
* responsibility to comply with third party license terms applicable to your
* use of third party software (including open source software) that may
* accompany Microchip software.
*
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
* EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
* WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
* PARTICULAR PURPOSE.
*
* IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
* INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
* WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
* BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
* FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
* ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*******************************************************************************/
//DOM-IGNORE-END

#ifndef _DRV_AT25DF_LOCAL_H
#define _DRV_AT25DF_LOCAL_H


// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************


// *****************************************************************************
// *****************************************************************************
// Section: Data Type Definitions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* DRV_AT25DF Command set

  Summary:
    Enumeration listing the DRV_AT25DF commands.

  Description:
    This enumeration defines the commands used to interact with the DRV_AT25DF
    series of devices.

  Remarks:
    None
*/

typedef enum
{
    /* Command to read JEDEC-ID of the flash device. */
    DRV_AT25DF_CMD_JEDEC_ID_READ      = 0x9F,

    /* Write enable command. */
    DRV_AT25DF_CMD_WRITE_ENABLE       = 0x06,

    /* Command to read Data Bytes */
    DRV_AT25DF_CMD_READ               = 0x03,

    /* Page Program command. */
    DRV_AT25DF_CMD_PAGE_PROGRAM       = 0x02,

    /* Command to read the Flash status register. */
    DRV_AT25DF_CMD_READ_STATUS_REG    = 0x05,

    /* Command to write the Flash status register. */
    DRV_AT25DF_CMD_WRITE_STATUS_REG   = 0x01,

    /* Command to perform sector erase */
    DRV_AT25DF_CMD_SECTOR_ERASE_4K    = 0x20,

    /* Command to perform Block erase */
    DRV_AT25DF_CMD_BLOCK_ERASE_64K    = 0xD8,

    /* Command to perform Chip erase */
    DRV_AT25DF_CMD_CHIP_ERASE         = 0xC7

} DRV_AT25DF_CMD;

typedef enum
{
    DRV_AT25DF_STATE_WRITE_EN,
    DRV_AT25DF_STATE_WRITE_CMD_ADDR,
    DRV_AT25DF_STATE_WRITE_DATA,
    DRV_AT25DF_STATE_CHECK_WRITE_STATUS,
    DRV_AT25DF_STATE_WAIT_WRITE_COMPLETE,
    DRV_AT25DF_STATE_READ_CMD_ADDR,
    DRV_AT25DF_STATE_READ_DATA,
    DRV_AT25DF_STATE_WAIT_READ_COMPLETE,
    DRV_AT25DF_STATE_ERASE_DATA,
    DRV_AT25DF_STATE_CHECK_ERASE_STATUS,
    DRV_AT25DF_STATE_WAIT_ERASE_COMPLETE,
    DRV_AT25DF_STATE_UNLOCK_FLASH,
    DRV_AT25DF_STATE_CHECK_UNLOCK_FLASH_STATUS,
    DRV_AT25DF_STATE_WAIT_UNLOCK_FLASH_COMPLETE,
    DRV_AT25DF_STATE_READ_JEDECID,
    DRV_AT25DF_STATE_WAIT_JEDECID_COMPLETE
}DRV_AT25DF_STATE;

// *****************************************************************************
/* AT25DF Driver Instance Object

  Summary:
    Object used to keep any data required for an instance of the AT25DF driver.

  Description:
    None.

  Remarks:
    None.
*/

typedef struct
{
    /* Flag to indicate this object is in use  */
    bool                            inUse;

    DRV_AT25DF_STATE                 state;

    /* Keep track of the number of clients
      that have opened this driver */
    size_t                          nClients;

    /* Maximum number of clients */
    size_t                          nClientsMax;

    /* The status of the driver */
    SYS_STATUS                      status;

    /* PLIB API list that will be used by the driver to access the hardware */
    const DRV_AT25DF_PLIB_INTERFACE*  spiPlib;

    uint8_t                         at25dfCommand[4];

    bool                            writeCompleted;

    SYS_PORT_PIN                    chipSelectPin;

    /* Points to the next FLASH memory address to write to */
    uint32_t                        memoryAddr;

    /* Pointer to the write buffer to write data from */
    uint8_t*                        bufferAddr;

    /* Pointer to the write buffer to write data from */
    uint32_t                        nPendingBytes;

    /* Page size information */
    uint32_t                        pageSize;

    /* Total flash size */
    uint32_t                        flashSize;

    uint32_t                        blockStartAddress;
	
    /* Flash Jedec ID */
    uint32_t                        jedecId;

    /* Application event handler */
    DRV_AT25DF_EVENT_HANDLER        eventHandler;

    /* Application context */
    uintptr_t                       context;

    volatile DRV_AT25DF_TRANSFER_STATUS       transferStatus;

} DRV_AT25DF_OBJ;


#endif //#ifndef _DRV_AT25DF_LOCAL_H
