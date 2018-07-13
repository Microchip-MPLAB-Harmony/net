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
Copyright (c) 2017 released Microchip Technology Inc.  All rights reserved.
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

#include <string.h>
#include "device.h"
#include "plib_efc0.h"

static uint32_t status = 0;

EFC_OBJECT efc;

bool EFC0_Read( uint32_t *data, uint32_t length, uint32_t address )
{
    memcpy((void *)data, (void *)address, length);
    return true;
}

bool EFC0_SectorErase( uint32_t address )
{
    volatile uint16_t page_number;

    /*Calculate the Page number to be passed for FARG register*/
    page_number = (address - IFLASH_ADDR) / IFLASH_PAGE_SIZE;
    /* Issue the FLASH erase operation*/
    EFC_REGS->EEFC_FCR = (EEFC_FCR_FCMD_EPA|EEFC_FCR_FARG(page_number|0x2)|EEFC_FCR_FKEY_PASSWD);

    status = 0;

    EFC_REGS->EEFC_FMR |= EEFC_FMR_FRDY_Msk;

    return true;
}

bool EFC0_PageWrite( uint32_t *data, uint32_t address )
{
    volatile uint16_t page_number;
    /*Calculate the Page number to be passed for FARG register*/
    page_number = (address - IFLASH_ADDR) / IFLASH_PAGE_SIZE;

    for (int i = 0; i < IFLASH_PAGE_SIZE; i += 4)
    {
    *((uint32_t *)( IFLASH_ADDR + ( page_number * IFLASH_PAGE_SIZE ) + i )) =    *(( data++ ));
    }

    __DSB();
    __ISB();

    /* Issue the FLASH write operation*/
    EFC_REGS->EEFC_FCR = (EEFC_FCR_FCMD_WP | EEFC_FCR_FARG(page_number)| EEFC_FCR_FKEY_PASSWD);

    status = 0;

    EFC_REGS->EEFC_FMR |= EEFC_FMR_FRDY_Msk;

    return true;
}

bool EFC0_QuadWordWrite( uint32_t *data, uint32_t address )
{
    volatile uint16_t page_number;

    /*Calculate the Page number to be passed for FARG register*/
    page_number = (address - IFLASH_ADDR) / IFLASH_PAGE_SIZE;

    for (int i = 0; i < 16; i += 4)
    {
    *((uint32_t *)(( address ) + i )) =    *((uint32_t *)( data++ ));
    }
    /* Issue the FLASH write operation*/
    EFC_REGS->EEFC_FCR = (EEFC_FCR_FCMD_WP | EEFC_FCR_FARG(page_number)| EEFC_FCR_FKEY_PASSWD);

    status = 0;

    EFC_REGS->EEFC_FMR |= EEFC_FMR_FRDY_Msk;

    return true;
}

void EFC0_RegionLock(uint32_t address)
{
    volatile uint16_t page_number;
    /*Calculate the Page number to be passed for FARG register*/
    page_number = (address - IFLASH_ADDR) / IFLASH_PAGE_SIZE;
    EFC_REGS->EEFC_FCR = (EEFC_FCR_FCMD_SLB | EEFC_FCR_FARG(page_number)| EEFC_FCR_FKEY_PASSWD);

    status = 0;

    EFC_REGS->EEFC_FMR |= EEFC_FMR_FRDY_Msk;
}

void EFC0_RegionUnlock(uint32_t address)
{
    volatile uint16_t page_number;
    /*Calculate the Page number to be passed for FARG register*/
    page_number = (address - IFLASH_ADDR) / IFLASH_PAGE_SIZE;
    EFC_REGS->EEFC_FCR = (EEFC_FCR_FCMD_CLB | EEFC_FCR_FARG(page_number)| EEFC_FCR_FKEY_PASSWD);

    status = 0;

    EFC_REGS->EEFC_FMR |= EEFC_FMR_FRDY_Msk;
}

bool EFC0_IsBusy(void)
{
    status |= EFC_REGS->EEFC_FSR;
    return (bool)(!(status & EEFC_FSR_FRDY_Msk));
}

EFC_ERROR EFC0_ErrorGet( void )
{
    status |= EFC_REGS->EEFC_FSR;
    return status;
}

void EFC0_CallbackRegister( EFC_CALLBACK callback, uintptr_t context )
{
    efc.callback = callback;
    efc.context = context;
}

void EFC0_InterruptHandler( void )
{
    uint32_t ul_fmr = EFC_REGS->EEFC_FMR;
    EFC_REGS->EEFC_FMR = ( ul_fmr & (~EEFC_FMR_FRDY_Msk));
    if(efc.callback != NULL)
        {
            efc.callback(efc.context);
        }
}
