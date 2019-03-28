/*******************************************************************************
  SDHC1 PLIB

  Company:
    Microchip Technology Inc.

  File Name:
    plib_sdhc1.h

  Summary:
    SDHC1 PLIB Header File

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

#ifndef PLIB_SDHC1_H
#define PLIB_SDHC1_H

#include "plib_sdhc_common.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

void SDHC1_BusWidthSet ( SDHC_BUS_WIDTH busWidth );

void SDHC1_SpeedModeSet ( SDHC_SPEED_MODE speedMode );

void SDHC1_BlockSizeSet ( uint16_t blockSize );

void SDHC1_BlockCountSet( uint16_t numBlocks );

bool SDHC1_IsCmdLineBusy ( void );

bool SDHC1_IsDatLineBusy ( void );

bool SDHC1_IsWriteProtected ( void );

bool SDHC1_IsCardAttached ( void );

bool SDHC1_ClockSet ( uint32_t clock);

void SDHC1_ClockEnable ( void );

void SDHC1_ClockDisable ( void );

uint16_t SDHC1_CommandErrorGet (void);

uint16_t SDHC1_DataErrorGet (void);

void SDHC1_ErrorReset ( SDHC_RESET_TYPE resetType );

void SDHC1_ResponseRead ( SDHC_READ_RESPONSE_REG respReg, uint32_t* response );

void SDHC1_ModuleInit ( void );

void SDHC1_Initialize( void );

void SDHC1_CallbackRegister(SDHC_CALLBACK callback, uintptr_t contextHandle);

void SDHC1_CommandSend (
    uint8_t opCode, 
    uint32_t argument,
    uint8_t respType, 
    SDHC_DataTransferFlags transferFlags
);

void SDHC1_DmaSetup (
    uint8_t* buffer,
    uint32_t numBytes,
    SDHC_DATA_TRANSFER_DIR direction
);

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    }

#endif
// DOM-IGNORE-END
#endif // PLIB_SDHC1_H

