/*******************************************************************************
  SDMMC0 PLIB

  Company:
    Microchip Technology Inc.

  File Name:
    plib_sdmmc0.h

  Summary:
    SDMMC0 PLIB Header File

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
#ifndef PLIB_SDMMC0_H
#define PLIB_SDMMC0_H

#include "plib_sdmmc_common.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END

#include <stdint.h>
#include <stdbool.h>

void SDMMC0_BusWidthSet ( SDMMC_BUS_WIDTH busWidth );

void SDMMC0_SpeedModeSet( SDMMC_SPEED_MODE speedMode );

void SDMMC0_BlockSizeSet ( uint16_t blockSize );

void SDMMC0_BlockCountSet( uint16_t numBlocks );

bool SDMMC0_IsCmdLineBusy ( void );

bool SDMMC0_IsDatLineBusy ( void );

bool SDMMC0_ClockSet ( uint32_t freq );

void SDMMC0_ClockEnable ( void );

void SDMMC0_ClockDisable ( void );

uint16_t SDMMC0_ErrorGet( void );

uint16_t SDMMC0_CommandErrorGet( void );

uint16_t SDMMC0_DataErrorGet( void );

void SDMMC0_ErrorReset ( SDMMC_RESET_TYPE resetType );

void SDMMC0_ResponseRead( SDMMC_READ_RESPONSE_REG respReg, uint32_t* response );

void SDMMC0_ModuleInit( void );

void SDMMC0_Initialize( void );

void SDMMC0_CallbackRegister( SDMMC_CALLBACK callback, uintptr_t contextHandle );

void SDMMC0_CommandSend(
    uint8_t opCode, 
    uint32_t argument,
    uint8_t respType, 
    SDMMC_DataTransferFlags transferFlags
);

void SDMMC0_DmaSetup(
    uint8_t* buffer,
    uint32_t numBytes,
    SDMMC_DATA_TRANSFER_DIR direction
);

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    }

#endif
// DOM-IGNORE-END
#endif // PLIB_SDMMC0_H

