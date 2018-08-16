/*******************************************************************************
  DMA System Service Mapping File

  Company:
    Microchip Technology Inc.

  File Name:
    sys_dma_mapping.h

  Summary:
    DMA System Service mapping file.

  Description:
    This header file contains the mapping of the APIs defined in the API header
    to either the function implementations or macro implementation or the
    specific variant implementation.
*******************************************************************************/

//DOM-IGNORE-BEGIN
/******************************************************************************
Copyright (c) 2017 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED 'AS IS' WITHOUT WARRANTY OF ANY KIND,
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
//DOM-IGNORE-END

#ifndef SYS_DMA_MAPPING_H
#define SYS_DMA_MAPPING_H

// *****************************************************************************
// *****************************************************************************
// Section: DMA System Service Mapping
// *****************************************************************************
// *****************************************************************************
#include "peripheral/xdmac/plib_xdmac.h"

#define SYS_DMA_ChannelCallbackRegister(channel, eventHandler, context)  XDMAC_ChannelCallbackRegister((XDMAC_CHANNEL)channel, (XDMAC_CHANNEL_CALLBACK)eventHandler, context)

#define SYS_DMA_ChannelTransfer(channel, srcAddr, destAddr, blockSize)  XDMAC_ChannelTransfer((XDMAC_CHANNEL)channel, srcAddr, destAddr, blockSize)

#define SYS_DMA_ChannelIsBusy(channel)  XDMAC_ChannelIsBusy((XDMAC_CHANNEL)channel)

#define SYS_DMA_ChannelDisable(channel)  XDMAC_ChannelDisable((XDMAC_CHANNEL)channel)

#endif // SYS_DMA_MAPPING_H
