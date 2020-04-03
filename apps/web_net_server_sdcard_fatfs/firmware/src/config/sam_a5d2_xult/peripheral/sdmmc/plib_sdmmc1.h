/*******************************************************************************
  SDMMC1 PLIB

  Company:
    Microchip Technology Inc.

  File Name:
    plib_sdmmc1.h

  Summary:
    SDMMC1 PLIB Header File

  Description:
    None

*******************************************************************************/

/*******************************************************************************
* Copyright (C) 2019 Microchip Technology Inc. and its subsidiaries.
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
#ifndef PLIB_SDMMC1_H
#define PLIB_SDMMC1_H

#include "plib_sdmmc_common.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END

#include <stdint.h>
#include <stdbool.h>

void SDMMC1_BusWidthSet ( SDMMC_BUS_WIDTH busWidth );

void SDMMC1_SpeedModeSet( SDMMC_SPEED_MODE speedMode );

void SDMMC1_BlockSizeSet ( uint16_t blockSize );

void SDMMC1_BlockCountSet( uint16_t numBlocks );

bool SDMMC1_IsCmdLineBusy ( void );

bool SDMMC1_IsDatLineBusy ( void );

bool SDMMC1_IsCardAttached ( void );

bool SDMMC1_ClockSet ( uint32_t freq );

void SDMMC1_ClockEnable ( void );

void SDMMC1_ClockDisable ( void );

uint16_t SDMMC1_ErrorGet( void );

uint16_t SDMMC1_CommandErrorGet( void );

uint16_t SDMMC1_DataErrorGet( void );

void SDMMC1_ErrorReset ( SDMMC_RESET_TYPE resetType );

void SDMMC1_ResponseRead( SDMMC_READ_RESPONSE_REG respReg, uint32_t* response );

void SDMMC1_ModuleInit( void );

void SDMMC1_Initialize( void );

void SDMMC1_CallbackRegister( SDMMC_CALLBACK callback, uintptr_t contextHandle );

void SDMMC1_CommandSend(
    uint8_t opCode, 
    uint32_t argument,
    uint8_t respType, 
    SDMMC_DataTransferFlags transferFlags
);

void SDMMC1_DmaSetup(
    uint8_t* buffer,
    uint32_t numBytes,
    SDMMC_DATA_TRANSFER_DIR direction
);

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    }

#endif
// DOM-IGNORE-END
#endif // PLIB_SDMMC1_H

