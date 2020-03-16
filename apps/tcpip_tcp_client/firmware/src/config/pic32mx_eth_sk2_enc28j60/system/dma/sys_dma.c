/*******************************************************************************
  DMA System Service Library Implementation Source File

  Company
    Microchip Technology Inc.

  File Name
    sys_dma.c

  Summary
    DMA system service library interface implementation.

  Description
    This file implements the interface to the DMA system service library.

  Remarks:
    DMA controller initialize will be done from within the MCC.

*******************************************************************************/

// DOM-IGNORE-BEGIN
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
// DOM-IGNORE-END


// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include "system/dma/sys_dma.h"

static SYS_DMA_CHANNEL_OBJECT  gSysDMAChannelObj[8];

//******************************************************************************
/* Function:
    void SYS_DMA_AddressingModeSetup(SYS_DMA_CHANNEL channel, SYS_DMA_SOURCE_ADDRESSING_MODE sourceAddrMode, SYS_DMA_DESTINATION_ADDRESSING_MODE destAddrMode);

  Summary:
    Setup addressing mode of selected DMA channel.

  Remarks:
    Check sys_dma.h for more info.
*/
void SYS_DMA_AddressingModeSetup(SYS_DMA_CHANNEL channel, SYS_DMA_SOURCE_ADDRESSING_MODE sourceAddrMode, SYS_DMA_DESTINATION_ADDRESSING_MODE destAddrMode)
{
    gSysDMAChannelObj[channel].srcAddrMode = sourceAddrMode;
    gSysDMAChannelObj[channel].destAddrMode = destAddrMode;
}

//******************************************************************************
/* Function:
    void SYS_DMA_DataWidthSetup(SYS_DMA_CHANNEL channel, SYS_DMA_WIDTH dataWidth);

  Summary:
    Setup data width of selected DMA channel.

  Remarks:
    Check sys_dma.h for more info.
*/
void SYS_DMA_DataWidthSetup(SYS_DMA_CHANNEL channel, SYS_DMA_WIDTH dataWidth)
{
    gSysDMAChannelObj[channel].dataWidth = dataWidth;
}

//******************************************************************************
/* Function:
   void SYS_DMA_ChannelTransfer(channel, srcAddr, destAddr, blockSize);

  Summary:
    Setup data width of selected DMA channel.

  Remarks:
    Check sys_dma.h for more info.
*/
bool SYS_DMA_ChannelTransfer (SYS_DMA_CHANNEL channel, const void *srcAddr, const void *destAddr, size_t blockSize)
{
    if ((gSysDMAChannelObj[channel].srcAddrMode == SYS_DMA_SOURCE_ADDRESSING_MODE_FIXED) && (gSysDMAChannelObj[channel].destAddrMode == SYS_DMA_DESTINATION_ADDRESSING_MODE_FIXED))
    {
        DMAC_ChannelTransfer((DMAC_CHANNEL)channel, srcAddr, gSysDMAChannelObj[channel].dataWidth, destAddr, gSysDMAChannelObj[channel].dataWidth, gSysDMAChannelObj[channel].dataWidth);
    }
    else if((gSysDMAChannelObj[channel].srcAddrMode == SYS_DMA_SOURCE_ADDRESSING_MODE_FIXED) && (gSysDMAChannelObj[channel].destAddrMode == SYS_DMA_DESTINATION_ADDRESSING_MODE_INCREMENTED))
    {
        DMAC_ChannelTransfer((DMAC_CHANNEL)channel, srcAddr, gSysDMAChannelObj[channel].dataWidth, destAddr, blockSize, gSysDMAChannelObj[channel].dataWidth);
    }
    else if ((gSysDMAChannelObj[channel].srcAddrMode == SYS_DMA_SOURCE_ADDRESSING_MODE_INCREMENTED) && (gSysDMAChannelObj[channel].destAddrMode == SYS_DMA_DESTINATION_ADDRESSING_MODE_FIXED))
    {
        DMAC_ChannelTransfer((DMAC_CHANNEL)channel, srcAddr, blockSize, destAddr, gSysDMAChannelObj[channel].dataWidth, gSysDMAChannelObj[channel].dataWidth);
    }
    else if ((gSysDMAChannelObj[channel].srcAddrMode == SYS_DMA_SOURCE_ADDRESSING_MODE_INCREMENTED) && (gSysDMAChannelObj[channel].destAddrMode == SYS_DMA_DESTINATION_ADDRESSING_MODE_INCREMENTED))
    {
        DMAC_ChannelTransfer((DMAC_CHANNEL)channel, srcAddr, blockSize, destAddr, blockSize, blockSize);
    }
    return true;
}