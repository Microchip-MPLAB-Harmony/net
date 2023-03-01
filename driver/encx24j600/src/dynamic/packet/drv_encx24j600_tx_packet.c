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
/*
Copyright (C) 2014-2023, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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


