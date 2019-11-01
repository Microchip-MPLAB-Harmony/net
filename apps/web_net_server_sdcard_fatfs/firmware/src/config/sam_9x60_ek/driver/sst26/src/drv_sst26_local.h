/*******************************************************************************
  SST26 Driver Local Data Structures

  Company:
    Microchip Technology Inc.

  File Name:
    drv_sst26_local.h

  Summary:
    SST26 driver local declarations and definitions

  Description:
    This file contains the SST26 driver's local declarations and definitions.
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

#ifndef _DRV_SST26_LOCAL_H
#define _DRV_SST26_LOCAL_H

// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************
#include <string.h>
#include "configuration.h"
#include "driver/sst26/drv_sst26.h"

// *****************************************************************************
// *****************************************************************************
// Section: Local Data Type Definitions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* SST26 Command set

  Summary:
    Enumeration listing the SST26VF commands.

  Description:
    This enumeration defines the commands used to interact with the SST26VF
    series of devices.

  Remarks:
    None
*/

typedef enum
{
    /* Reset enable command. */
    SST26_CMD_FLASH_RESET_ENABLE = 0x66,

    /* Command to reset the flash. */
    SST26_CMD_FLASH_RESET        = 0x99,

    /* Command to Enable QUAD IO */
    SST26_CMD_ENABLE_QUAD_IO     = 0x38,

    /* Command to Reset QUAD IO */
    SST26_CMD_RESET_QUAD_IO      = 0xFF,

    /* Command to read JEDEC-ID of the flash device. */
    SST26_CMD_JEDEC_ID_READ      = 0x9F,

    /* QUAD Command to read JEDEC-ID of the flash device. */
    SST26_CMD_QUAD_JEDEC_ID_READ = 0xAF,

    /* Command to perform High Speed Read */
    SST26_CMD_HIGH_SPEED_READ    = 0x0B,

    /* Write enable command. */
    SST26_CMD_WRITE_ENABLE       = 0x06,

    /* Page Program command. */
    SST26_CMD_PAGE_PROGRAM       = 0x02,

    /* Command to read the Flash status register. */
    SST26_CMD_READ_STATUS_REG    = 0x05,

    /* Command to perform sector erase */
    SST26_CMD_SECTOR_ERASE       = 0x20,

    /* Command to perform Bulk erase */
    SST26_CMD_BULK_ERASE_64K     = 0xD8,

    /* Command to perform Chip erase */
    SST26_CMD_CHIP_ERASE         = 0xC7,

    /* Command to unlock the flash device. */
    SST26_CMD_UNPROTECT_GLOBAL   = 0x98

} SST26_CMD;

// *****************************************************************************
/* SST26 Driver operations.

  Summary:
    Enumeration listing the SST26 driver operations.

  Description:
    This enumeration defines the possible SST26 driver operations.

  Remarks:
    None
*/

typedef enum
{
    /* Request is a command operation */
    DRV_SST26_OPERATION_TYPE_CMD = 0,

    /* Request is read operation. */
    DRV_SST26_OPERATION_TYPE_READ,

    /* Request is write operation. */
    DRV_SST26_OPERATION_TYPE_WRITE,

    /* Request is erase operation. */
    DRV_SST26_OPERATION_TYPE_ERASE,

} DRV_SST26_OPERATION_TYPE;

/**************************************
 * SST26 Driver Hardware Instance Object
 **************************************/
typedef struct
{
    /* Flag to indicate in use  */
    bool inUse;

    /* Flag to indicate status of transfer */
    volatile bool isTransferDone;

    /* The status of the driver */
    SYS_STATUS status;

    /* Intent of opening the driver */
    DRV_IO_INTENT ioIntent;

    /* Indicates the number of clients that have opened this driver */
    size_t nClients;

    /* Current Operation */
    DRV_SST26_OPERATION_TYPE curOpType;

    /* PLIB API list that will be used by the driver to access the hardware */
    const DRV_SST26_PLIB_INTERFACE *sst26Plib;
} DRV_SST26_OBJECT;

#endif //#ifndef _DRV_SST26_LOCAL_H

/*******************************************************************************
 End of File
*/

