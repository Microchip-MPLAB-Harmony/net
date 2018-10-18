/*******************************************************************************
  DMA System Service Library Interface Header File

  Company
    Microchip Technology Inc.

  File Name
    sys_dma.h

  Summary
    DMA system service library interface.

  Description
    This file defines the interface to the DMA system service library.
    This library provides access to and control of the DMA controller.

  Remarks:
    DMA controller initialize will be done from within the MCC.

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
// DOM-IGNORE-END

#ifndef SYS_DMA_H    // Guards against multiple inclusion
#define SYS_DMA_H


// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

/*  This section lists the other files that are included in this file.
*/
#include <stddef.h>
#include "system/system.h"

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
/* DMA Channels

  Summary:
    This lists the set of channels available for data transfer using DMA.

  Description:
    This lists the set of channels available for data transfer using DMA.

  Remarks:
    None.
*/

typedef enum {

    DMA_CHANNEL_0,
    DMA_CHANNEL_1,
    DMA_CHANNEL_2,
    DMA_CHANNEL_3,
    DMA_CHANNEL_4,
    DMA_CHANNEL_5,
    DMA_CHANNEL_6,
    DMA_CHANNEL_7,
    DMA_CHANNEL_8,
    DMA_CHANNEL_9,
    DMA_CHANNEL_10,
    DMA_CHANNEL_11,
    DMA_CHANNEL_12,
    DMA_CHANNEL_13,
    DMA_CHANNEL_14,
    DMA_CHANNEL_15,
    DMA_CHANNEL_16,
    DMA_CHANNEL_17,
    DMA_CHANNEL_18,
    DMA_CHANNEL_19,
    DMA_CHANNEL_20,
    DMA_CHANNEL_21,
    DMA_CHANNEL_22,
    DMA_CHANNEL_23,
    DMA_CHANNELS_NUMBER

} DMA_CHANNEL;


// *****************************************************************************
/* DMA System Service Channel None

  Summary:
    DMA channel none

  Description:
    This constant identifies the specification of no choice from client
    for allocating a particular DMA channel.

  Remarks:
    This constant should be used in place of hard-coded numeric literals.
*/
#define DMA_CHANNEL_NONE  /*DOM-IGNORE-BEGIN*/((DMA_CHANNEL)-1)/*DOM-IGNORE-END*/


// *****************************************************************************
/* DMA Transfer Events

   Summary:
    Enumeration of possible DMA transfer events.

   Description:
    This data type provides an enumeration of all possible DMA transfer
    events.

   Remarks:
    None.

*/
typedef enum
{
    /* Data was transferred successfully. */
    SYS_DMA_TRANSFER_COMPLETE,

    /* Error while processing the request */
    SYS_DMA_TRANSFER_ERROR

} SYS_DMA_TRANSFER_EVENT;


// *****************************************************************************
/* DMA Transfer Event Handler Function

   Summary:
    Pointer to a DMA Transfer Event handler function.

   Description:
    This data type defines a DMA Transfer Event Handler Function.

    A DMA client must register a transfer event handler function of this
    type to receive transfer related events from the DMA System Service.

    If the event is SYS_DMA_TRANSFER_COMPLETE, this means that the data
    was transferred successfully.

    If the event is SYS_DMA_TRANSFER_ERROR, this means that the data was
    not transferred successfully.

    The contextHandle parameter contains the context handle that was provided by
    the client at the time of registering the event handler. This context handle
    can be anything that the client consider helpful or necessary to identify
    the client context object associated with the channel of the DMA that
    generated the event.

    The event handler function executes in an interrupt context of DMA.
    It is recommended to the application not to perform process intensive
    operations with in this function.

   Remarks:
    None.

*/
typedef void (*SYS_DMA_CHANNEL_CALLBACK) (SYS_DMA_TRANSFER_EVENT event, uintptr_t contextHandle);


// *****************************************************************************
// *****************************************************************************
// Section: Interface Routines
// *****************************************************************************
// *****************************************************************************
/* The following functions make up the methods (set of possible operations) of
   this interface.
*/

//******************************************************************************
/*
  Function:
    void SYS_DMA_ChannelCallbackRegister
    (
        DMA_CHANNEL channel,
        const SYS_DMA_CHANNEL_CALLBACK eventHandler,
        const uintptr_t contextHandle
    )

  Summary:
    This function allows a DMA client to set an event handler.

  Description:
    This function allows a client to set an event handler. The client may want
    to receive transfer related events in cases when it submits a DMA
    transfer request. The event handler should be set before the client
    intends to perform operations that could generate events.

    This function accepts a contextHandle parameter. This parameter could be
    set by the client to contain (or point to) any client specific data object
    that should be associated with this DMA channel.

  Precondition:
    DMA Controller should have been initialized.

  Parameters:
    channel - A specific DMA channel from which the events are expected.

    eventHandler - Pointer to the event handler function.

    contextHandle - Value identifying the context of the
    application/driver/middleware that registered the event handling function.

  Returns:
    None.

  Example:
    <code>
    MY_APP_OBJ myAppObj;

    void APP_DMA_TransferEventHandler(SYS_DMA_TRANSFER_EVENT event,
            uintptr_t contextHandle)
    {
        switch(event)
        {
            case SYS_DMA_TRANSFER_COMPLETE:
                // This means the data was transferred.
                break;

            case SYS_DMA_TRANSFER_ERROR:
                // Error handling here.
                break;

            default:
                break;
        }
    }

    // User registers an event handler with DMA channel. This is done once.
    SYS_DMA_ChannelCallbackRegister(channel, APP_DMA_TransferEventHandler,
            (uintptr_t)&myAppObj);
    </code>

  Remarks:
    None.
 */

void SYS_DMA_ChannelCallbackRegister (DMA_CHANNEL channel, const SYS_DMA_CHANNEL_CALLBACK eventHandler, const uintptr_t contextHandle);


//******************************************************************************
/* Function:
    void SYS_DMA_ChannelTransfer
    (
        DMA_CHANNEL channel,
        const void *srcAddr,
        const void *destAddr,
        size_t blockSize
    )

  Summary:
    Adds a data transfer to a DMA channel and enables the channel to start
    data transfer.

  Description:
    This function adds a single block data transfer characteristics for a
    specific DMA channel. It also enables the channel to start data transfer.

    If the requesting client registered an event callback with the PLIB,
    the PLIB will issue a SYS_DMA_TRANSFER_COMPLETE event if the transfer was
    processed successfully and SYS_DMA_TRANSFER_ERROR event if the transfer was
    not processed successfully.

  Precondition:
    DMA Controller should have been initialized.

  Parameters:
    channel - A specific DMA channel

    srcAddr - Source of the DMA transfer

    destAddr - Destination of the DMA transfer

    blockSize - Size of the transfer block

  Returns:
    None.

  Example:
    <code>
    // Transfer 10 bytes of data to UART TX using DMA channel 1
    MY_APP_OBJ myAppObj;
    uint8_t buf[10] = {0,1,2,3,4,5,6,7,8,9};
    void *srcAddr = (uint8_t *) buf;
    void *destAddr = (uin8_t*) &U1TXREG;
    size_t size = 10;

    // User registers an event handler with PLIB. This is done once.
    SYS_DMA_ChannelCallbackRegister(APP_DMA_TransferEventHandler,
        (uintptr_t)&myAppObj);

    SYS_DMA_ChannelTransfer(DMA_CHANNEL_1, srcAddr, destAddr, size);
    </code>

  Remarks:
    The source/destination buffers should be made coherent and aligned to the
    device cache line size to avoid the cache coherency issues.
    For example:
    <code>
    uint8_t buffer[1024];
    uint8_t __attribute__((coherent)) __attribute__((aligned(32))) buffer[1024];
    </code>
*/

void SYS_DMA_ChannelTransfer (DMA_CHANNEL channel, const void *srcAddr, const void *destAddr, size_t blockSize);


//******************************************************************************
/* Function:
    bool SYS_DMA_ChannelIsBusy (DMA_CHANNEL channel)

  Summary:
    Returns the busy status of a specific DMA Channel.

  Description:
    This function returns the busy status of the DMA channel.
    DMA channel will be busy if any transfer is in progress.

    This function can be used to check the status of the channel prior to
    submitting a transfer request. And this can also be used to check the status
    of the submitted request if callback mechanism is not preferred.

  Precondition:
    DMA Controller should have been initialized.

  Parameters:
    channel - A specific DMA channel

  Returns:
    Busy status of the specific channel.
    True - Channel is busy
    False - Channel is free

  Example:
    <code>
    // Transfer 10 bytes of data to UART TX using DMA channel 1
    MY_APP_OBJ myAppObj;
    uint8_t buf[10] = {0,1,2,3,4,5,6,7,8,9};
    void *srcAddr = (uint8_t *) buf;
    void *destAddr = (uin8_t*) &U1TXREG;
    size_t size = 10;

    if(false == SYS_DMA_ChannelIsBusy(DMA_CHANNEL_1))
    {
        SYS_DMA_ChannelTransfer(DMA_CHANNEL_1, srcAddr, destAddr, size);
    }
    </code>

  Remarks:
    None.
*/

bool SYS_DMA_ChannelIsBusy (DMA_CHANNEL channel);


//******************************************************************************
/* Function:
    void SYS_DMA_ChannelDisable (DMA_CHANNEL channel)

  Summary:
    Disables the specified channel.

  Description:
    This function disables the specified channel and resets it.
    Ongoing transfer will be aborted.

  Precondition:
    DMA Controller should have been initialized.

  Parameters:
    channel - A specific DMA channel

  Returns:
    None.

  Example:
    <code>
    SYS_DMA_ChannelDisable(DMA_CHANNEL_1);
    </code>

  Remarks:
    None.
*/

void SYS_DMA_ChannelDisable (DMA_CHANNEL channel);


#include "sys_dma_mapping.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    }

#endif
// DOM-IGNORE-END

#endif //SYS_DMA_H
