/*******************************************************************************
  MX25L Driver Local Data Structures

  Company:
    Microchip Technology Inc.

  File Name:
    drv_mx25l_local.h

  Summary:
    MX25L driver local declarations and definitions

  Description:
    This file contains the MX25L driver's local declarations and definitions.
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

#ifndef _DRV_MX25L_LOCAL_H
#define _DRV_MX25L_LOCAL_H

// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************
#include <string.h>
#include "configuration.h"
#include "driver/mx25l/drv_mx25l.h"

// *****************************************************************************
// *****************************************************************************
// Section: Version Numbers
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
// *****************************************************************************
// Section: Local Data Type Definitions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* MX25L Command set

  Summary:
    Enumeration listing the MX25L commands.

  Description:
    This enumeration defines the commands used to interact with the MX25L
    series of devices.

  Remarks:
    None
*/

typedef enum
{
    /* Reset enable command. */
    MX25L_CMD_FLASH_RESET_ENABLE = 0x66,

    /* Command to reset the flash. */
    MX25L_CMD_FLASH_RESET        = 0x99,

    /* Command to Enable QUAD IO */
    MX25L_CMD_ENABLE_QUAD_IO     = 0x35,

    /* Command to Reset QUAD IO */
    MX25L_CMD_RESET_QUAD_IO      = 0xF5,

    /* Command to read JEDEC-ID of the flash device. */
    MX25L_CMD_JEDEC_ID_READ      = 0x9F,

    /* QUAD Command to read JEDEC-ID of the flash device. */
    MX25L_CMD_QUAD_JEDEC_ID_READ = 0xAF,

    /* Command to perform High Speed Read */
    MX25L_CMD_HIGH_SPEED_QREAD    = 0xEB,

    /* Write enable command. */
    MX25L_CMD_WRITE_ENABLE       = 0x06,

    /* Page Program command. */
    MX25L_CMD_PAGE_PROGRAM       = 0x02,

    /* Command to read the Flash status register. */
    MX25L_CMD_READ_STATUS_REG    = 0x05,

    /* Command to perform sector erase */
    MX25L_CMD_SECTOR_ERASE       = 0x20,

    /* Command to perform Block erase */
    MX25L_CMD_BLOCK_ERASE_64K    = 0xD8,

    /* Command to perform Chip erase */
    MX25L_CMD_CHIP_ERASE         = 0xC7,

    /* Command to unlock the flash device. */
    MX25L_CMD_UNPROTECT_GLOBAL   = 0x98

} MX25L_CMD;

// *****************************************************************************
/* MX25L Driver operations.

  Summary:
    Enumeration listing the MX25L driver operations.

  Description:
    This enumeration defines the possible MX25L driver operations.

  Remarks:
    None
*/

typedef enum
{
    /* Request is read operation. */
    DRV_MX25L_OPERATION_TYPE_READ = 0,

    /* Request is write operation. */
    DRV_MX25L_OPERATION_TYPE_WRITE,

    /* Request is erase operation. */
    DRV_MX25L_OPERATION_TYPE_ERASE,

} DRV_MX25L_OPERATION_TYPE;

/**************************************
 * MX25L Driver Hardware Instance Object
 **************************************/
typedef struct
{
    /* Flag to indicate in use  */
    bool inUse;

    /* The status of the driver */
    SYS_STATUS status;

    /* Current Operation */
    DRV_MX25L_OPERATION_TYPE curOpType;

    /* PLIB API list that will be used by the driver to access the hardware */
    const DRV_MX25L_PLIB_INTERFACE *mx25lPlib;
} DRV_MX25L_OBJECT;

#endif //#ifndef _DRV_MX25L_LOCAL_H

/*******************************************************************************
 End of File
*/

