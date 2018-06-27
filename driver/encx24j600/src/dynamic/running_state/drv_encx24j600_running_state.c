/*******************************************************************************
  ENCx24J600 Running State Machine
  Company:
    Microchip Technology Inc.

  File Name:
    drv_encx24j600_running_state.c
  Summary:

  Description:
*******************************************************************************/
// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2014 released Microchip Technology Inc.  All rights reserved.
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
#include "drv_encx24j600_running_state.h"
#include "../drv_encx24j600_local.h"
#include "../drv_encx24j600_utils.h"

int32_t DRV_ENCX24J600_RunningStateTask(struct _DRV_ENCX24J600_DriverInfo * pDrvInst)
{
    uint8_t loops;
    DRV_ENCX24J600_RUNNING_STATE_INFO * curSt = &(pDrvInst->mainStateInfo.runningInfo);
    uint8_t count;

    for (loops = 0; loops < 5; loops++)
    {
        bool running = false;
    if (curSt->ctsToEnc && !TCPIP_Helper_ProtectedSingleListIsEmpty(&pDrvInst->txPendingPackets))
    {
        for (count = 0; count < MAX_TX_DESCRIPTORS; count++)
        {
            if (pDrvInst->txDescriptors[count].state == DRV_ENCX24J600_TP_NO_PKT_STATE)
            {
                pDrvInst->txDescriptors[count].pkt = (TCPIP_MAC_PACKET*)TCPIP_Helper_ProtectedSingleListHeadRemove(&pDrvInst->txPendingPackets);
                pDrvInst->txDescriptors[count].state = DRV_ENCX24J600_TP_READ_GPWRREG;
                curSt->ctsToEnc = false;
                DRV_ENCX24J600_TxPacketTask(pDrvInst, &pDrvInst->txDescriptors[count]);
                break;
            }
        }
    }
    if (curSt->ctTx)
    {
        for (count = 0; count < MAX_TX_DESCRIPTORS; count++)
        {
            if (pDrvInst->txDescriptors[count].state == DRV_ENCX24J600_TP_WAIT_FOR_CTTX)
            {
                pDrvInst->txDescriptors[count].state = DRV_ENCX24J600_TP_SET_ETXST;
                curSt->ctTx = false;
                DRV_ENCX24J600_TxPacketTask(pDrvInst, &pDrvInst->txDescriptors[count]);
                break;
            }
        }
    }

    DRV_ENCX24J600_ChkIntStateTask(pDrvInst);

    if (curSt->ctrFromEnc && pDrvInst->rxPacketPending && !TCPIP_Helper_ProtectedSingleListIsEmpty(&pDrvInst->rxFreePackets))
    {
        if (pDrvInst->rxDescriptors.state == DRV_ENCX24J600_RX_EMPTY_PACKET)
        {
            pDrvInst->rxDescriptors.pkt = (TCPIP_MAC_PACKET*)TCPIP_Helper_ProtectedSingleListHeadRemove(&pDrvInst->rxFreePackets);
            pDrvInst->rxDescriptors.state = DRV_ENCX24J600_RX_SET_ERXRDPTR;
            DRV_ENCX24J600_RxPacketTask(pDrvInst, &pDrvInst->rxDescriptors);
        }
    }

    DRV_ENCX24J600_ChkStatusStateTask(pDrvInst);
    DRV_ENCX24J600_ChgDupStateTask(pDrvInst);
    DRV_ENCX24J600_ChkTxStatusStateTask(pDrvInst);
    DRV_ENCX24J600_ResetRxStateTask(pDrvInst);

    if (pDrvInst->rxDescriptors.state != DRV_ENCX24J600_RX_EMPTY_PACKET)
    {
        DRV_ENCX24J600_RxPacketTask(pDrvInst, &pDrvInst->rxDescriptors);
        running = true;
    }

    for (count = 0; count < MAX_TX_DESCRIPTORS; count++)
    {
        if (pDrvInst->txDescriptors[count].state != DRV_ENCX24J600_TP_NO_PKT_STATE)
        {
            DRV_ENCX24J600_TxPacketTask(pDrvInst, &pDrvInst->txDescriptors[count]);
            running = true;
        }
    }
    if (!running)
    {
        return 0;
    }
    }
    return 0;
}
int32_t DRV_ENCX24J600_RunningStateEnter(struct _DRV_ENCX24J600_DriverInfo * pDrvInst)
{
    DRV_ENCX24J600_RUNNING_STATE_INFO * curSt = &(pDrvInst->mainStateInfo.runningInfo);
    curSt->ctTx = true;
    curSt->ctrFromEnc = true;
    curSt->ctsToEnc = true;
    curSt->nTxOkPackets = 0;
    curSt->nRxOkPackets = 0;
    DRV_ENCX24J600_ChkIntStateEnter(pDrvInst);
    DRV_ENCX24J600_ChkStatusStateEnter(pDrvInst);
    DRV_ENCX24J600_ChgDupStateEnter(pDrvInst);
    uint8_t count;
    for (count = 0; count < MAX_TX_DESCRIPTORS; count++)
    {
        DRV_ENCX24J600_TxPacketEnter(pDrvInst, &pDrvInst->txDescriptors[count]);
    }
    DRV_ENCX24J600_RxPacketEnter(pDrvInst, &pDrvInst->rxDescriptors);
    return 0;
}
int32_t DRV_ENCX24J600_RunningStateExit(struct _DRV_ENCX24J600_DriverInfo * pDrvInst)
{
    DRV_ENCX24J600_ChkIntStateExit(pDrvInst);
    DRV_ENCX24J600_ChkStatusStateExit(pDrvInst);
    DRV_ENCX24J600_ChgDupStateExit(pDrvInst);
    uint8_t count;
    for (count = 0; count < MAX_TX_DESCRIPTORS; count++)
    {
        DRV_ENCX24J600_TxPacketExit(pDrvInst, &pDrvInst->txDescriptors[count]);
    }
    DRV_ENCX24J600_RxPacketExit(pDrvInst, &pDrvInst->rxDescriptors);
    return 0;
}
