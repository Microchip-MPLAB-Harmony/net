/*******************************************************************************
  DBGU PLIB

  Company:
    Microchip Technology Inc.

  File Name:
    plib_dbgu.h

  Summary:
    DBGU PLIB Global Header File

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

#ifndef PLIB_DBGU_COMMON_H
#define PLIB_DBGU_COMMON_H

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include "device.h" //For DBGU_*_Msk

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

typedef enum
{
    DBGU_ERROR_NONE = 0,
    DBGU_ERROR_OVERRUN = DBGU_SR_OVRE_Msk,
    DBGU_ERROR_PARITY = DBGU_SR_PARE_Msk,
    DBGU_ERROR_FRAMING = DBGU_SR_FRAME_Msk

} DBGU_ERROR;

typedef enum
{
    DBGU_PARITY_NONE = DBGU_MR_PAR_NO,

    DBGU_PARITY_ODD = DBGU_MR_PAR_ODD,

    DBGU_PARITY_EVEN = DBGU_MR_PAR_EVEN,

    DBGU_PARITY_MARK = DBGU_MR_PAR_MARK,

    DBGU_PARITY_SPACE = DBGU_MR_PAR_SPACE,

    /* Force the compiler to reserve 32-bit space for each enum */
    DBGU_PARITY_INVALID = 0xFFFFFFFF

} DBGU_PARITY;

typedef struct
{
    uint32_t baudRate;

    DBGU_PARITY parity;

} DBGU_SERIAL_SETUP;

typedef void (* DBGU_CALLBACK)( uintptr_t context );


// *****************************************************************************
// *****************************************************************************
// Section: Local: **** Do Not Use ****
// *****************************************************************************
// *****************************************************************************

typedef struct
{
    uint8_t *               txBuffer;
    size_t                  txSize;
    size_t                  txProcessedSize;
    DBGU_CALLBACK           txCallback;
    uintptr_t               txContext;
    bool                    txBusyStatus;

    uint8_t *               rxBuffer;
    size_t                  rxSize;
    size_t                  rxProcessedSize;
    DBGU_CALLBACK           rxCallback;
    uintptr_t               rxContext;
    bool                    rxBusyStatus;

} DBGU_OBJECT ;

typedef enum
{
    /* Threshold number of bytes are available in the receive ring buffer */
    DBGU_EVENT_READ_THRESHOLD_REACHED = 0,

    /* Receive ring buffer is full. Application must read the data out to avoid missing data on the next RX interrupt. */
    DBGU_EVENT_READ_BUFFER_FULL,

    /* DBGU error. Application must call the DBGU_ErrorGet API to get the type of error and clear the error. */
    DBGU_EVENT_READ_ERROR,

    /* Threshold number of free space is available in the transmit ring buffer */
    DBGU_EVENT_WRITE_THRESHOLD_REACHED,
}DBGU_EVENT;

typedef void (* DBGU_RING_BUFFER_CALLBACK)(DBGU_EVENT event, uintptr_t context );


// *****************************************************************************
// *****************************************************************************
// Section: Local: **** Do Not Use ****
// *****************************************************************************
// *****************************************************************************

typedef struct
{
    DBGU_RING_BUFFER_CALLBACK                           wrCallback;

    uintptr_t                                           wrContext;

    volatile uint32_t                                   wrInIndex;

    volatile uint32_t                                   wrOutIndex;

    bool                                                isWrNotificationEnabled;

    uint32_t                                            wrThreshold;

    bool                                                isWrNotifyPersistently;

    DBGU_RING_BUFFER_CALLBACK                           rdCallback;

    uintptr_t                                           rdContext;

    volatile uint32_t                                   rdInIndex;

    volatile uint32_t                                   rdOutIndex;

    bool                                                isRdNotificationEnabled;

    uint32_t                                            rdThreshold;

    bool                                                isRdNotifyPersistently;

} DBGU_RING_BUFFER_OBJECT;

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    }

#endif
// DOM-IGNORE-END
#endif // PLIB_DBGU_COMMON_H
