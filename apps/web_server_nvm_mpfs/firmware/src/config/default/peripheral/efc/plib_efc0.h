/*******************************************************************************
 Interface definition of EFC PLIB.
 
 Company:
    Microchip Technology Inc.
    
 File Name:
    plib_EFC.h
    
 Summary:
    Interface definition of EFC Plib.
    
 Description:
    This file defines the interface for the EFC Plib.
    It allows user to Program, Erase and lock the on-chip FLASH memory.
*******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2016 released Microchip Technology Inc.  All rights reserved.
Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).
You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.
SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
*******************************************************************************/
// DOM-IGNORE-END

#ifndef EFC0_H    // Guards against multiple inclusion
#define EFC0_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// DOM-IGNORE-BEGIN
#ifdef __cplusplus // Provide C++ Compatibility
    extern "C" {
#endif
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Interface
// *****************************************************************************
// *****************************************************************************

#define EFC0_SECTORSIZE              0x2000
#define EFC0_PAGESIZE                0x200
#define EFC0_LOCKSIZE                0x4000
#define EFC0_START_ADDRESS           0x500000
#define EFC0_MEDIA_SIZE              1024
#define EFC0_ERASE_BUFFER_SIZE       8192


typedef enum
{
    EFC_ERROR_NONE = 0x1,
    /*In-valid command*/
    EFC_CMD_ERROR = 0x2,
    /*Flash region is locked*/
    EFC_LOCK_ERROR = 0x4,
    /*Flash Error*/
    EFC_FLERR_ERROR = 0x8,
    /*Flash Encountered an ECC error*/
    EFC_ECC_ERROR = 0xF0000,
} EFC_ERROR;

typedef void (*EFC_CALLBACK)(uintptr_t context);
typedef struct
{
    EFC_CALLBACK          callback;
    uintptr_t               context;
} EFC_OBJECT ;

bool EFC0_Read( uint32_t *data, uint32_t length, uint32_t address );

bool EFC0_SectorErase( uint32_t address );

bool EFC0_PageWrite( uint32_t *data, uint32_t address );

bool EFC0_QuadWordWrite( uint32_t *data, uint32_t address );

EFC_ERROR EFC0_ErrorGet( void );

bool EFC0_IsBusy(void);

void EFC0_RegionLock(uint32_t address);

void EFC0_RegionUnlock(uint32_t address);

void EFC0_CallbackRegister( EFC_CALLBACK callback, uintptr_t context );

// DOM-IGNORE-BEGIN
#ifdef __cplusplus // Provide C++ Compatibility
}
#endif
// DOM-IGNORE-END

#endif