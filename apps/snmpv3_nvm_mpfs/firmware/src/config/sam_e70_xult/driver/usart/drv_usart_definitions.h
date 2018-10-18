/*******************************************************************************
  USART Driver Definitions Header File

  Company:
    Microchip Technology Inc.

  File Name:
    drv_usart_definitions.h

  Summary:
    USART Driver Definitions Header File

  Description:
    This file provides implementation-specific definitions for the USART
    driver's system interface.
*******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2017 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED AS IS  WITHOUT  WARRANTY  OF  ANY  KIND,
EITHER EXPRESS  OR  IMPLIED,  INCLUDING  WITHOUT  LIMITATION,  ANY  WARRANTY  OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A  PARTICULAR  PURPOSE.
IN NO EVENT SHALL MICROCHIP OR  ITS  LICENSORS  BE  LIABLE  OR  OBLIGATED  UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION,  BREACH  OF  WARRANTY,  OR
OTHER LEGAL  EQUITABLE  THEORY  ANY  DIRECT  OR  INDIRECT  DAMAGES  OR  EXPENSES
INCLUDING BUT NOT LIMITED TO ANY  INCIDENTAL,  SPECIAL,  INDIRECT,  PUNITIVE  OR
CONSEQUENTIAL DAMAGES, LOST  PROFITS  OR  LOST  DATA,  COST  OF  PROCUREMENT  OF
SUBSTITUTE  GOODS,  TECHNOLOGY,  SERVICES,  OR  ANY  CLAIMS  BY  THIRD   PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE  THEREOF),  OR  OTHER  SIMILAR  COSTS.
*******************************************************************************/
//DOM-IGNORE-END

#ifndef DRV_USART_DEFINITIONS_H
#define DRV_USART_DEFINITIONS_H

// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************
#include "system/int/sys_int.h"
#include "system/dma/sys_dma.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* USART Driver Errors Declaration */

typedef enum _DRV_USART_ERROR
{
    DRV_USART_ERROR_NONE = 0,

    DRV_USART_ERROR_OVERRUN = 1,

    DRV_USART_ERROR_PARITY = 2,

    DRV_USART_ERROR_FRAMING = 4

}_DRV_USART_ERROR;

// *****************************************************************************
/* USART Serial Setup */

typedef enum
{
    DRV_USART_DATA_5_BIT = 0,

    DRV_USART_DATA_6_BIT = 1,

    DRV_USART_DATA_7_BIT = 2,

    DRV_USART_DATA_8_BIT = 3,

    DRV_USART_DATA_9_BIT = 4

} DRV_USART_DATA_BIT;

typedef enum
{
    DRV_USART_PARITY_NONE = 0,

    DRV_USART_PARITY_ODD = 1,

    DRV_USART_PARITY_EVEN = 2,

    DRV_USART_PARITY_MARK = 3,

    DRV_USART_PARITY_SPACE = 4

} DRV_USART_PARITY;

typedef enum
{
    DRV_USART_STOP_1_BIT = 0,

    DRV_USART_STOP_1_5_BIT = 1,

    DRV_USART_STOP_2_BIT = 2

} DRV_USART_STOP_BIT;

typedef struct _DRV_USART_SERIAL_SETUP
{
    uint32_t baudRate;

    DRV_USART_DATA_BIT dataWidth;

    DRV_USART_PARITY parity;

    DRV_USART_STOP_BIT stopBits;

} _DRV_USART_SERIAL_SETUP;

// *****************************************************************************
/* USART PLIB API Set needed by the driver */

typedef bool(*USART_ReadCallbackRegister)(void * callback, uintptr_t context);
typedef size_t(*USART_Read)(void *buffer, const size_t size);
typedef bool(*USART_ReadIsBusy)(void);
typedef size_t(*USART_ReadCountGet)(void);

typedef bool(*USART_WriteCallbackRegister)(void * callback, uintptr_t context);
typedef size_t(*USART_Write)(void *buffer, const size_t size);
typedef bool(*USART_WriteIsBusy)(void);
typedef size_t(*USART_WriteCountGet)(void);

typedef _DRV_USART_ERROR(*USART_ErrorGet)(void);
typedef bool(*USART_SerialSetup)(_DRV_USART_SERIAL_SETUP* setup, uint32_t clkSrc);

typedef struct
{
    USART_ReadCallbackRegister readCallbackRegister;
    USART_Read read;
    USART_ReadIsBusy readIsBusy;
    USART_ReadCountGet readCountGet;

    USART_WriteCallbackRegister writeCallbackRegister;
    USART_Write write;
    USART_WriteIsBusy writeIsBusy;
    USART_WriteCountGet writeCountGet;

    USART_ErrorGet errorGet;
    USART_SerialSetup serialSetup;

} USART_PLIB_API;


// *****************************************************************************
/* USART Driver Initialization Data Declaration */

struct _DRV_USART_INIT
{
    /* Identifies the PLIB API set to be used by the driver to access the
     * peripheral. */
    USART_PLIB_API *usartPlib;

    /* This is the USART transmit DMA channel. */
    DMA_CHANNEL dmaChannelTransmit;

    /* This is the USART receive DMA channel. */
    DMA_CHANNEL dmaChannelReceive;

    /* This is the USART transmit register address. Used for DMA operation. */
    void * usartTransmitAddress;

    /* This is the USART receive register address. Used for DMA operation. */
    void * usartReceiveAddress;

    /* This is the receive buffer queue size. This is the maximum
     * number of read requests that driver will queue. This can be updated
     * through DRV_USART_RCV_QUEUE_SIZE_IDXn macro in configuration.h */
    unsigned int queueSizeReceive;

    /* This is the transmit buffer queue size. This is the maximum
     * number of write requests that driver will queue. This can be updated
     * through DRV_USART_XMIT_QUEUE_SIZE_IDXn macro in configuration.h */
    unsigned int queueSizeTransmit;

    /* Interrupt source ID for the USART interrupt. */
    INT_SOURCE interruptUSART;

    /* This is the DMA channel interrupt source. */
    INT_SOURCE interruptDMA;

};


//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END


#endif // #ifndef DRV_USART_DEFINITIONS_H

