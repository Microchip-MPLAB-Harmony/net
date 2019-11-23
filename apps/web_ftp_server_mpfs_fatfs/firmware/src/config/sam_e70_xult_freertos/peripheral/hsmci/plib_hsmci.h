/*******************************************************************************
  HSMCI PLIB

  Company:
    Microchip Technology Inc.

  File Name:
    plib_hsmci.h

  Summary:
    HSMCI PLIB Header File

  Description:
    None

*******************************************************************************/

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

#ifndef PLIB_HSMCI_H
#define PLIB_HSMCI_H

#include "plib_hsmci_common.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

void HSMCI_BusWidthSet ( HSMCI_BUS_WIDTH busWidth );

void HSMCI_SpeedModeSet ( HSMCI_SPEED_MODE speedMode );

void HSMCI_BlockSizeSet ( uint16_t blockSize );

void HSMCI_BlockCountSet ( uint16_t numBlocks );

bool HSMCI_IsCmdLineBusy (void);

bool HSMCI_IsDatLineBusy (void);

void HSMCI_ClockSet ( uint32_t clock );

uint16_t HSMCI_CommandErrorGet(void);

uint16_t HSMCI_DataErrorGet(void);

void HSMCI_ResponseRead ( HSMCI_READ_RESPONSE_REG respReg, uint32_t* response );

void HSMCI_Initialize( void );

void HSMCI_ModuleInit( void );

void HSMCI_CallbackRegister(HSMCI_CALLBACK callback, uintptr_t contextHandle);

void HSMCI_CommandSend ( 
    uint8_t opCode, 
    uint32_t argument,
    uint8_t respType, 
    HSMCI_DataTransferFlags transferFlags
);

void HSMCI_DmaSetup
(
    uint8_t* buffer,
    uint32_t numBytes,
    HSMCI_DATA_TRANSFER_DIR direction
);

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    }

#endif
// DOM-IGNORE-END
#endif // PLIB_HSMCI_H

