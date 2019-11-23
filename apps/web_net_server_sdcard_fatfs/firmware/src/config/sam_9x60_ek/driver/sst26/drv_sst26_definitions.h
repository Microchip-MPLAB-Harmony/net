/*******************************************************************************
  SST26 Driver Definitions Header File

  Company:
    Microchip Technology Inc.

  File Name:
    drv_sst26_definitions.h

  Summary:
    SST26 Driver Definitions Header File

  Description:
    This file provides implementation-specific definitions for the SST26
	driver's system interface.
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

#ifndef DRV_SST26_DEFINITIONS_H
#define DRV_SST26_DEFINITIONS_H

// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************

#include "system/system.h"
#include "driver/driver.h"
#include "peripheral/qspi/plib_qspi.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility
    extern "C" {
#endif
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
// *****************************************************************************

/* QSPI PLIB API Set

  Summary:
  The set of PLIB APIs used by the SST26 driver.

  Description:
  The API set holds the function names available at the PLIb level for the
  corresponding functionality. Driver may call these functions to make use of
  the features provided by the PLIB.

  Remarks:
    None.
*/

/* Pointer to write command to QSPI slave device. */
typedef bool (*DRV_SST26_PLIB_CMD_WRITE)( qspi_command_xfer_t *qspi_command_xfer, uint32_t address );

/* Pointer to read particular register of QSPI slave device. */
typedef bool (*DRV_SST26_PLIB_REG_READ)( qspi_register_xfer_t *qspi_register_xfer, uint32_t *rx_data, uint8_t rx_data_length );

/* Pointer to write to particular register of QSPI slave device. */
typedef bool (*DRV_SST26_PLIB_REG_WRITE)( qspi_register_xfer_t *qspi_register_xfer, uint32_t *tx_data, uint8_t tx_data_length );

/* Pointer to read from the specified address of the flash device. */
typedef bool (*DRV_SST26_PLIB_MEM_READ)( qspi_memory_xfer_t *qspi_memory_xfer, uint32_t *rx_data, uint32_t rx_data_length, uint32_t address );

/* Pointer to write to the specified address of the flash device. */
typedef bool (*DRV_SST26_PLIB_MEM_WRITE)( qspi_memory_xfer_t *qspi_memory_xfer, uint32_t *tx_data, uint32_t tx_data_length, uint32_t address );

typedef struct
{
    /* Pointer to write command to QSPI slave device. */
    DRV_SST26_PLIB_CMD_WRITE CommandWrite;

    /* Pointer to read particular register of QSPI slave device. */
    DRV_SST26_PLIB_REG_READ RegisterRead;

    /* Pointer to write to particular register of QSPI slave device. */
    DRV_SST26_PLIB_REG_WRITE RegisterWrite;

    /* Pointer to read from the specified address of the flash device. */
    DRV_SST26_PLIB_MEM_READ MemoryRead;

    /* Pointer to write to the specified address of the flash device. */
    DRV_SST26_PLIB_MEM_WRITE MemoryWrite;

} DRV_SST26_PLIB_INTERFACE;

/* SST26 Driver Initialization Data Declaration */

typedef struct
{
    /* Identifies the PLIB API set to be used by the driver to access the
     * peripheral. */
    const DRV_SST26_PLIB_INTERFACE *sst26Plib;
} DRV_SST26_INIT;


//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END


#endif // #ifndef DRV_SST26_DEFINITIONS_H
