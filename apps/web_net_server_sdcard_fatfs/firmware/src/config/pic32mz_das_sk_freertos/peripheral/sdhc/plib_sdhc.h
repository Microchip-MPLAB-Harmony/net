/*******************************************************************************
  SDHC PLIB

  Company:
    Microchip Technology Inc.

  File Name:
    plib_sdhc.h

  Summary:
    SDHC PLIB Header File

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

#ifndef PLIB_SDHC_H
#define PLIB_SDHC_H

#include "plib_sdhc_common.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

void SDHC_BusWidthSet ( SDHC_BUS_WIDTH busWidth );

void SDHC_SpeedModeSet ( SDHC_SPEED_MODE speedMode );

void SDHC_BlockSizeSet ( uint16_t blockSize );

void SDHC_BlockCountSet( uint16_t numBlocks );

bool SDHC_IsCmdLineBusy ( void );

bool SDHC_IsDatLineBusy ( void );

bool SDHC_IsWriteProtected ( void );

void SDHC_CardDetectEnable( void );

void SDHC_CardDetectDisable( void );

void SDHC_WriteProtectEnable( void );

void SDHC_WriteProtectDisable( void );

bool SDHC_IsCardAttached ( void );

bool SDHC_ClockSet ( uint32_t clock);

void SDHC_ClockEnable ( void );

void SDHC_ClockDisable ( void );

uint16_t SDHC_CommandErrorGet (void);

uint16_t SDHC_DataErrorGet (void);

void SDHC_ErrorReset ( SDHC_RESET_TYPE resetType );

void SDHC_ResponseRead ( SDHC_READ_RESPONSE_REG respReg, uint32_t* response );

void SDHC_ModuleInit ( void );

void SDHC_Initialize( void );

void SDHC_CallbackRegister(SDHC_CALLBACK callback, uintptr_t contextHandle);

void SDHC_CommandSend (
    uint8_t opCode, 
    uint32_t argument,
    uint8_t respType, 
    SDHC_DataTransferFlags transferFlags
);

void SDHC_DmaSetup (
    uint8_t* buffer,
    uint32_t numBytes,
    SDHC_DATA_TRANSFER_DIR direction
);

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    }

#endif
// DOM-IGNORE-END
#endif // PLIB_SDHC_H

