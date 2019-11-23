/*******************************************************************************
  DBGU PLIB

  Company:
    Microchip Technology Inc.

  File Name:
    plib_dbgu.h

  Summary:
    DBGU PLIB Header File

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

#ifndef PLIB_DBGU_H
#define PLIB_DBGU_H

#include "plib_dbgu_common.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Interface
// *****************************************************************************
// *****************************************************************************
#define DBGU_FrequencyGet()    (uint32_t)(200000000UL)

/****************************** DBGU API *********************************/

void DBGU_Initialize(void);

DBGU_ERROR DBGU_ErrorGet(void);

bool DBGU_SerialSetup(DBGU_SERIAL_SETUP *setup, uint32_t srcClkFreq);

bool DBGU_Write(void *buffer, const size_t size);

bool DBGU_Read(void *buffer, const size_t size);

bool DBGU_WriteIsBusy(void);

bool DBGU_ReadIsBusy(void);

size_t DBGU_WriteCountGet(void);

size_t DBGU_ReadCountGet(void);

void DBGU_WriteCallbackRegister(DBGU_CALLBACK callback, uintptr_t context);

void DBGU_ReadCallbackRegister(DBGU_CALLBACK callback, uintptr_t context);


// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    }

#endif
// DOM-IGNORE-END
#endif // PLIB_DBGU_H
