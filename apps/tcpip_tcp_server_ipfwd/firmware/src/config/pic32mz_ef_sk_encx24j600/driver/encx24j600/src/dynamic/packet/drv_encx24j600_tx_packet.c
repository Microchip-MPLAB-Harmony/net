/*******************************************************************************
  ENCx24J600 TX Packet and State Machine
  Company:
    Microchip Technology Inc.

  File Name:
    drv_encx24j600_tx_packet.c
  Summary:

  Description:
*******************************************************************************/
// DOM-IGNORE-BEGIN
/*****************************************************************************
 Copyright (C) 2014-2018 Microchip Technology Inc. and its subsidiaries.

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
#include "drv_encx24j600_tx_packet.h"
#include "../drv_encx24j600_local.h"
#include "../drv_encx24j600_utils.h"
#include "../drv_encx24j600_ds_defs.h"
#include "../running_state/drv_encx24j600_running_state.h"

uint16_t lastPacketAddr = 0xffff;

int32_t DRV_ENCX24J600_TxPacketTask(struct _DRV_ENCX24J600_DriverInfo * pDrvInst, DRV_ENCX24J600_TX_PACKET_INFO *pkt)
{
    DRV_ENCX24J600_RegUnion reg = {0};
    uintptr_t ret;
    switch (pkt->state)
    {
        case DRV_ENCX24J600_TP_NO_PKT_STATE:
            break;
        case DRV_ENCX24J600_TP_WAIT_FOR_CTS_TO_ENC:
            break;
        case DRV_ENCX24J600_TP_READ_GPWRREG:
        {
            ret = (pDrvInst->busVTable->fpPtrRdStart)(pDrvInst, DRV_ENCX24J600_PTR_GPWR, DRV_ENCX24J500_TP_OP_READ_GPWRREG);
            if (ret != 0)
            {
                pkt->operation = ret;
                pkt->state = DRV_ENCX24J600_TP_SND_PKT_TO_ENC;
            }
            else
            {
                break;
            }
        }
        case DRV_ENCX24J600_TP_SND_PKT_TO_ENC:
        {
            if (pkt->gpPtr == DRV_ENCX24J600_MEM_SIZE)
            {
                pkt->gpPtr = pDrvInst->gpPtrVal;
                pkt->pDSeg = pkt->pkt->pDSeg;
            }
            uint16_t count = 0;
            while (pkt->pDSeg != NULL)
            {

                if (pDrvInst->txBufferRemaining < pkt->pDSeg->segLen)
                {
                    return 0;
                }
                ret = (pDrvInst->busVTable->fpDataWr)(pDrvInst, DRV_ENCX24J600_PTR_GPWR, pkt->pDSeg->segLoad, pkt->pDSeg->segLen);
                count+= pkt->pDSeg->segLen;
                if (ret == 0)
                {
                    return 0;
                }
                DRV_ENCX24J600_AddGpData(pDrvInst, pkt->pDSeg->segLen);
                pkt->pDSeg = pkt->pDSeg->next;
            }
            // If we're here there are no data segments left.
            //SYS_CONSOLE_PRINT("TXe:pkt %x @ %x len %x next @ %x\r\n", pkt, pkt->gpPtr, count, pDrvInst->gpPtrVal);
            pkt->state = DRV_ENCX24J600_TP_WAIT_FOR_GPWRREG;
        }
        case DRV_ENCX24J600_TP_WAIT_FOR_GPWRREG:
        {
            uint16_t ptrValue = 0;
            if ((*pDrvInst->busVTable->fpOpResult)(pDrvInst, pkt->operation) == DRV_ENCX24J600_BR_SUCCESS)
            {
                (*pDrvInst->busVTable->fpPtrRdResult)(pDrvInst, pkt->operation, &ptrValue, DRV_ENCX24J500_TP_OP_READ_GPWRREG);
                /*if (pkt->gpPtr != ptrValue)
                {
                    SYS_CONSOLE_PRINT("%x Change in pointer %x %x\r\n", pkt, pkt->gpPtr, ptrValue);
                }*/
                pkt->gpPtr = ptrValue;
                pkt->state = DRV_ENCX24J600_TP_WAIT_FOR_CTTX;
                pDrvInst->mainStateInfo.runningInfo.ctsToEnc = true;
            }
            else
            {
                break;
            }
        }
        case DRV_ENCX24J600_TP_WAIT_FOR_CTTX:
            break;

        case DRV_ENCX24J600_TP_SET_ETXST:
        {
            reg.value = 0;
            reg.etxst.ETXST = pkt->gpPtr;
            ret = (*pDrvInst->busVTable->fpSfrWr)(pDrvInst, DRV_ENCX24J600_SFR_ETXS, reg, DRV_ENCX24J600_TP_OP_SET_ETXST);
            if (ret == 0)
            {
                break;
            }
            pkt->state = DRV_ENCX24J600_TP_SET_ETXLEN;
        }
        case DRV_ENCX24J600_TP_SET_ETXLEN:
        {
            reg.value = 0;
            uint16_t count = 0;
            pkt->pDSeg = pkt->pkt->pDSeg;
            while (pkt->pDSeg != NULL)
            {
                count += pkt->pDSeg->segLen;
                pkt->pDSeg = pkt->pDSeg->next;
            }
            reg.etxlen.ETXLEN = count;
            ret = (*pDrvInst->busVTable->fpSfrWr)(pDrvInst, DRV_ENCX24J600_SFR_ETXLEN, reg, DRV_ENCX24J600_TP_OP_SET_ETXLEN);
            if (ret == 0)
            {
                break;
            }
            //SYS_CONSOLE_PRINT("TX:pkt %x @ %x len %x next @ %x\r\n", pkt, pkt->gpPtr, count, pDrvInst->gpPtrVal);
            /*if (lastPacketAddr == pkt->gpPtr)
            {
                SYS_CONSOLE_MESSAGE("Badd Stuff\r\n");
            }*/
            lastPacketAddr = pkt->gpPtr;
            pkt->state = DRV_ENCX24J600_TP_SET_ETXLEN;
        }
        case DRV_ENCX24J600_TP_RQ_PKT_TX:
        {
            ret = (*pDrvInst->busVTable->fpReqPktTx)(pDrvInst);
            if (ret == 0)
            {
                break;
            }
            pkt->state = DRV_ENCX24J600_TP_WAIT_FOR_COMPLETE;
        }
        case DRV_ENCX24J600_TP_WAIT_FOR_COMPLETE:
            break;
        case DRV_ENCX24J600_TP_RST_EIR:
        {
            if(pkt->pkt != NULL)
            {
                uint16_t count = 0;
                pkt->pDSeg = pkt->pkt->pDSeg;
                while (pkt->pDSeg != NULL)
                {
                    count += pkt->pDSeg->segLen;
                    pkt->pDSeg = pkt->pDSeg->next;
                }
                if (!pDrvInst->mainStateInfo.runningInfo.chkStaInfo.linkState)
                {
                    pkt->pkt->ackRes = TCPIP_MAC_PKT_ACK_LINK_DOWN;
                }
                else
                {
                     pkt->pkt->ackRes = TCPIP_MAC_PKT_ACK_TX_OK;
                }

                pkt->pkt->pktFlags &= ~TCPIP_MAC_PKT_FLAG_QUEUED;
                (*pkt->pkt->ackFunc)(pkt->pkt, pkt->pkt->ackParam);
                DRV_ENCX24J600_SetEvent(pDrvInst, TCPIP_MAC_EV_TX_DONE);
                pkt->pkt = NULL;
                DRV_ENCX24J600_TxAck(pDrvInst, count);
                pDrvInst->mainStateInfo.runningInfo.nTxOkPackets ++;
                pkt->gpPtr = DRV_ENCX24J600_MEM_SIZE;
            }
            reg.value = 0;
            reg.eir.TXABTIF = 1;
            reg.eir.TXIF = 1;
            ret = (*pDrvInst->busVTable->fpSfrBitClr)(pDrvInst, DRV_ENCX24J600_SFR_EIR, reg, DRV_ENCX24J600_TP_OP_RST_EIR);
            if (ret == 0)
            {
                break;
            }
             pkt->state = DRV_ENCX24J600_TP_NO_PKT_STATE;
             pDrvInst->mainStateInfo.runningInfo.ctTx = true;
             pkt->pkt = NULL;
             pkt->gpPtr = DRV_ENCX24J600_MEM_SIZE;
        }
        break;
    }
    return 0;
}

int32_t DRV_ENCX24J600_TxPacketEnter(struct _DRV_ENCX24J600_DriverInfo * pDrvInst, DRV_ENCX24J600_TX_PACKET_INFO *pkt)
{
    pkt->state = DRV_ENCX24J600_TP_NO_PKT_STATE;
    pkt->pkt = NULL;
    pkt->gpPtr = DRV_ENCX24J600_MEM_SIZE;
    return 0;
}

int32_t DRV_ENCX24J600_TxPacketExit(struct _DRV_ENCX24J600_DriverInfo * pDrvInst, DRV_ENCX24J600_TX_PACKET_INFO *pkt)
{
    return 0;
}


