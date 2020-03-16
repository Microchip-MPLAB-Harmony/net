/*******************************************************************************
  ENC28J60 Running State Machine
  Company:
    Microchip Technology Inc.

  File Name:
    drv_enc28j60_running_state.c
  Summary:

  Description:
*******************************************************************************/
// DOM-IGNORE-BEGIN
/*****************************************************************************
 Copyright (C) 2015-2018 Microchip Technology Inc. and its subsidiaries.

Microchip Technology Inc. and its subsidiaries.

Subject to your compliance with these terms, you may use Microchip software 
and any derivatives exclusively with Microchip products. It is your 
responsibility to comply with third party license terms applicable to your 
use of third party software (including open source software) that may 
accompany Microchip software.

THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER 
EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED 
WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR 
PURPOSE.

IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS 
BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE 
FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN 
ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY, 
THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*****************************************************************************/

// DOM-IGNORE-END
#include "drv_enc28j60_running_state.h"
#include "../drv_enc28j60_local.h"
#include "../drv_enc28j60_utils.h"




int32_t DRV_ENC28J60_RunningStateTask(struct _DRV_ENC28J60_DriverInfo * pDrvInst)
{
    uint8_t loops;
    DRV_ENC28J60_RUNNING_STATE_INFO * curSt = &(pDrvInst->mainStateInfo.runningInfo);
    uint8_t count;

    for (loops = 0; loops < 5; loops++)
    {
        bool running = false;

        if (curSt->ctsToEnc && !TCPIP_Helper_ProtectedSingleListIsEmpty(&pDrvInst->txPendingPackets))
        {
            for (count = 0; count < MAX_TX_DESCRIPTORS; count++)
            {
                if (pDrvInst->txDescriptors[count].state == DRV_ENC28J60_TP_NO_PKT_STATE)
                {
                    pDrvInst->txDescriptors[count].macPkt = (TCPIP_MAC_PACKET*)TCPIP_Helper_ProtectedSingleListHeadRemove(&pDrvInst->txPendingPackets);
                    pDrvInst->txDescriptors[count].state = DRV_ENC28J60_TP_WRITE_TXST;
                    curSt->ctsToEnc = false;
                    DRV_ENC28J60_TxPacketTask(pDrvInst, &pDrvInst->txDescriptors[count]);
                    break;
                }
            }
        }


        if (curSt->ctTx)
        {
            for (count = 0; count < MAX_TX_DESCRIPTORS; count++)
            {
                if (pDrvInst->txDescriptors[count].state == DRV_ENC28J60_TP_WAIT_FOR_CTTX)
                {
                    pDrvInst->txDescriptors[count].state = DRV_ENC28J60_TP_RQ_PKT_TX;
                    curSt->ctTx = false;
                    DRV_ENC28J60_TxPacketTask(pDrvInst, &pDrvInst->txDescriptors[count]);
                    break;
                }
            }
        }

        DRV_ENC28J60_ChkIntStateTask(pDrvInst);


        if (curSt->ctrFromEnc && pDrvInst->rxPacketPending && !TCPIP_Helper_ProtectedSingleListIsEmpty(&pDrvInst->rxFreePackets))
        {
            if (pDrvInst->rxDescriptors.state == DRV_ENC28J60_RX_EMPTY_PACKET)
            {
                pDrvInst->rxDescriptors.macPkt = (TCPIP_MAC_PACKET*)TCPIP_Helper_ProtectedSingleListHeadRemove(&pDrvInst->rxFreePackets);
                pDrvInst->rxDescriptors.state = DRV_ENC28J60_RX_SET_ERDPTR;
                DRV_ENC28J60_RxPacketTask(pDrvInst, &pDrvInst->rxDescriptors);
            }
        }

        DRV_ENC28J60_ChkStatusStateTask(pDrvInst);
        DRV_ENC28J60_ChkTxStatusStateTask(pDrvInst);
        DRV_ENC28J60_ResetRxStateTask(pDrvInst);

        if (pDrvInst->rxDescriptors.state != DRV_ENC28J60_RX_EMPTY_PACKET)
        {
            DRV_ENC28J60_RxPacketTask(pDrvInst, &pDrvInst->rxDescriptors);
            running = true;
        }

        for (count = 0; count < MAX_TX_DESCRIPTORS; count++)
        {
            if (pDrvInst->txDescriptors[count].state != DRV_ENC28J60_TP_NO_PKT_STATE)
            {
                DRV_ENC28J60_TxPacketTask(pDrvInst, &pDrvInst->txDescriptors[count]);
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


int32_t DRV_ENC28J60_RunningStateEnter(struct _DRV_ENC28J60_DriverInfo * pDrvInst)
{
    DRV_ENC28J60_RUNNING_STATE_INFO * curSt = &(pDrvInst->mainStateInfo.runningInfo);
    curSt->ctTx = true;
    curSt->ctrFromEnc = true;
    curSt->ctsToEnc = true;
    curSt->nTxOkPackets = 0;
    curSt->nRxOkPackets = 0;
    DRV_ENC28J60_ChkIntStateEnter(pDrvInst);
    DRV_ENC28J60_ChkStatusStateEnter(pDrvInst);
    uint8_t count;
    for (count = 0; count < MAX_TX_DESCRIPTORS; count++)
    {
        DRV_ENC28J60_TxPacketEnter(pDrvInst, &pDrvInst->txDescriptors[count]);
    }
    DRV_ENC28J60_RxPacketEnter(pDrvInst, &pDrvInst->rxDescriptors);
    return 0;
}

int32_t DRV_ENC28J60_RunningStateExit(struct _DRV_ENC28J60_DriverInfo * pDrvInst)
{
    DRV_ENC28J60_ChkIntStateExit(pDrvInst);
    DRV_ENC28J60_ChkStatusStateExit(pDrvInst);
    uint8_t count;
    for (count = 0; count < MAX_TX_DESCRIPTORS; count++)
    {
        DRV_ENC28J60_TxPacketExit(pDrvInst, &pDrvInst->txDescriptors[count]);
    }
    DRV_ENC28J60_RxPacketExit(pDrvInst, &pDrvInst->rxDescriptors);
    return 0;
}

