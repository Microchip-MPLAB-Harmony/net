/*******************************************************************************
Interface definition of QSPI PLIB.

 Company:
    Microchip Technology Inc.

 File Name:
    plib_qspi_common.h

 Summary:
    Interface definition of the Quad Serial Peripheral Interface Plib (QSPI).

 Description:
    This file defines the interface for the QSPI Plib.
    It allows user to setup QSPI and transfer data to and from slave devices
    attached.
*******************************************************************************/

// DOM-IGNORE-BEGIN
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
// DOM-IGNORE-END

#ifndef PLIB_QSPI_COMMON_H // Guards against multiple inclusion
#define PLIB_QSPI_COMMON_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

/* This section lists the other files that are included in this file.
*/

#include <stdbool.h>

// DOM-IGNORE-BEGIN
#ifdef __cplusplus // Provide C++ Compatibility
    extern "C" {
#endif
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
// *****************************************************************************

typedef enum
{
    ADDRL_24_BIT = QSPI_IFR_ADDRL_24_BIT,
    ADDRL_32_BIT = QSPI_IFR_ADDRL_32_BIT
} QSPI_ADDRESS_LENGTH;

typedef enum
{
    SINGLE_BIT_SPI = QSPI_IFR_WIDTH_SINGLE_BIT_SPI,
    DUAL_OUTPUT = QSPI_IFR_WIDTH_DUAL_OUTPUT,
    QUAD_OUTPUT = QSPI_IFR_WIDTH_QUAD_OUTPUT,
    DUAL_IO = QSPI_IFR_WIDTH_DUAL_IO,
    QUAD_IO = QSPI_IFR_WIDTH_QUAD_IO,
    DUAL_CMD = QSPI_IFR_WIDTH_DUAL_CMD,
    QUAD_CMD = QSPI_IFR_WIDTH_QUAD_CMD	
} QSPI_LANE_WIDTH;

typedef enum
{
    OPTL_1_BIT = QSPI_IFR_OPTL_OPTION_1BIT,
    OPTL_2_BIT = QSPI_IFR_OPTL_OPTION_2BIT,
    OPTL_4_BIT = QSPI_IFR_OPTL_OPTION_4BIT,
    OPTL_8_BIT = QSPI_IFR_OPTL_OPTION_8BIT
} QSPI_OPTION_LENGTH;

typedef enum
{
    QSPI_REG_READ,
    QSPI_REG_WRITE,
    QSPI_MEM_READ,
    QSPI_MEM_WRITE
} QSPI_TRANSFER_TYPE;

typedef struct {
    /* QSPI instruction code */
    uint8_t instruction;
    /* QSPI instruction Frame register informations */
    QSPI_LANE_WIDTH width;
    bool addr_en;
    QSPI_ADDRESS_LENGTH addr_len;
} qspi_command_xfer_t;

typedef struct {
    /* QSPI instruction code */
    uint8_t instruction;
    /* QSPI instruction Frame register informations */
    QSPI_LANE_WIDTH width;
    /* For Read Register */
    uint8_t dummy_cycles;
} qspi_register_xfer_t;

typedef struct {
    /* QSPI instruction code */
    uint8_t instruction;
    /* QSPI option code */
    uint8_t option;
    /* QSPI instruction Frame register informations */
    QSPI_LANE_WIDTH width;
    QSPI_ADDRESS_LENGTH addr_len;
    bool option_en;
    QSPI_OPTION_LENGTH option_len;
    /* For Read memory */
    bool continuous_read_en;
    uint8_t dummy_cycles;
} qspi_memory_xfer_t;


// DOM-IGNORE-BEGIN
#ifdef __cplusplus // Provide C++ Compatibility
}
#endif
// DOM-IGNORE-END

#endif /* PLIB_QSPI_COMMON_H */
