/*******************************************************************************
  ENCx24J600 Driver Utilities
  Company:
    Microchip Technology Inc.

  File Name:
    drv_encx24j600_utils.h
  Summary:

  Description:
*******************************************************************************/
// DOM-IGNORE-BEGIN
/*
Copyright (C) 2014-2025, Microchip Technology Inc., and its subsidiaries. All rights reserved.

The software and documentation is provided by microchip and its contributors
"as is" and any express, implied or statutory warranties, including, but not
limited to, the implied warranties of merchantability, fitness for a particular
purpose and non-infringement of third party intellectual property rights are
disclaimed to the fullest extent permitted by law. In no event shall microchip
or its contributors be liable for any direct, indirect, incidental, special,
exemplary, or consequential damages (including, but not limited to, procurement
of substitute goods or services; loss of use, data, or profits; or business
interruption) however caused and on any theory of liability, whether in contract,
strict liability, or tort (including negligence or otherwise) arising in any way
out of the use of the software and documentation, even if advised of the
possibility of such damage.

Except as expressly permitted hereunder and subject to the applicable license terms
for any third-party software incorporated in the software and any applicable open
source software license terms, no license or other rights, whether express or
implied, are granted under any patent or other intellectual property rights of
Microchip or any third party.
*/

// DOM-IGNORE-END

#ifndef H_DRV_ENCX24J600_UTILS_H_
#define H_DRV_ENCX24J600_UTILS_H_

#ifdef __cplusplus
extern "C" {
#endif
    
#include "drv_encx24j600_ds_defs.h"
#include "drv_encx24j600_local.h"
#include "tcpip/tcpip_mac.h"
#include "system_config.h"  
    
bool DRV_ENCX24J600_ReadSfr(struct S_DRV_ENCX24J600_DriverInfo *  pDrvInst, uintptr_t  handle, DRV_ENCX24J600_RegUnion *  value,  uint8_t  opIndex );

void DRV_ENCX24J600_SetEvent(struct S_DRV_ENCX24J600_DriverInfo *  pDrvInst, TCPIP_MAC_EVENT events);

void DRV_ENCX24J600_AddGpData(struct S_DRV_ENCX24J600_DriverInfo *  pDrvInst, uint16_t size);

static inline void DRV_ENCX24J600_TxAck(struct S_DRV_ENCX24J600_DriverInfo *  pDrvInst, uint16_t size)
{
    pDrvInst->txBufferRemaining += size;
}

// helpers, conversion functions
//
static __inline__ TCPIP_MAC_PACKET* __attribute__((always_inline)) FC_SglNode2MacPkt(SGL_LIST_NODE* node)
{
    union
    {
        SGL_LIST_NODE* node;
        TCPIP_MAC_PACKET* pMacPkt;
    }U_SGL_NODE_MAC_PKT;

    U_SGL_NODE_MAC_PKT.node = node;
    return U_SGL_NODE_MAC_PKT.pMacPkt;
}

static __inline__ SGL_LIST_NODE* __attribute__((always_inline)) FC_MacPkt2SglNode(TCPIP_MAC_PACKET* pMacPkt)
{
    union
    {
        TCPIP_MAC_PACKET* pMacPkt;
        SGL_LIST_NODE* node;
    }U_MAC_PKT_SGL_NODE;

    U_MAC_PKT_SGL_NODE.pMacPkt = pMacPkt;
    return U_MAC_PKT_SGL_NODE.node;
}

static __inline__ struct S_DRV_ENCX24J600_DriverInfo* __attribute__((always_inline)) FC_Param2DrvInfo(const void* param)
{
    union
    {
        const void* param;
        struct S_DRV_ENCX24J600_DriverInfo* drvInfo;
    }U_PARAM_DRV_INFO;

    U_PARAM_DRV_INFO.param = param;
    return U_PARAM_DRV_INFO.drvInfo;
}


#ifdef __cplusplus
}
#endif


#endif  // H_DRV_ENCX24J600_UTILS_H_

