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
#include "drv_encx24j600_tx_packet.h"
#include "../drv_encx24j600_local.h"
#include "../drv_encx24j600_utils.h"
#include "../drv_encx24j600_ds_defs.h"
#include "../running_state/drv_encx24j600_running_state.h"

static uint16_t lastPacketAddr = 0xffffU;

int32_t DRV_ENCX24J600_TxPacketTask(struct S_DRV_ENCX24J600_DriverInfo * pDrvInst, DRV_ENCX24J600_TX_PACKET_INFO *pkt)
{
    DRV_ENCX24J600_RegUnion reg = {0};
    uintptr_t ret;
    uint16_t count;
    uint16_t ptrValue;
    bool abortTask = false;

    while(!abortTask)
    {
        switch (pkt->state)
        {
            case DRV_ENCX24J600_TP_NO_PKT_STATE:
                abortTask = true;
                break;

            case DRV_ENCX24J600_TP_WAIT_FOR_CTS_TO_ENC:
                abortTask = true;
                break;

            case DRV_ENCX24J600_TP_READ_GPWRREG:
                ret = (pDrvInst->busVTable->fpPtrRdStart)(pDrvInst, DRV_ENCX24J600_PTR_GPWR, DRV_ENCX24J500_TP_OP_READ_GPWRREG);
                if (ret != 0)
                {
                    pkt->operation = ret;
                    pkt->state = DRV_ENCX24J600_TP_SND_PKT_TO_ENC;
                }
                else
                {
                    abortTask = true;
                }
                break;

            case DRV_ENCX24J600_TP_SND_PKT_TO_ENC:
                if (pkt->gpPtr == DRV_ENCX24J600_MEM_SIZE)
                {
                    pkt->gpPtr = pDrvInst->gpPtrVal;
                    pkt->pDSeg = pkt->pkt->pDSeg;
                }
                count = 0;
                while (pkt->pDSeg != NULL)
                {
                    if (pDrvInst->txBufferRemaining < pkt->pDSeg->segLen)
                    {
                        abortTask = true;
                        break;
                    }
                    ret = (pDrvInst->busVTable->fpDataWr)(pDrvInst, DRV_ENCX24J600_PTR_GPWR, pkt->pDSeg->segLoad, pkt->pDSeg->segLen);
                    count+= pkt->pDSeg->segLen;
                    if (ret == 0)
                    {
                        abortTask = true;
                        break;
                    }
                    DRV_ENCX24J600_AddGpData(pDrvInst, pkt->pDSeg->segLen);
                    pkt->pDSeg = pkt->pDSeg->next;
                }

                if(abortTask)
                {
                    break;
                }

                // If we're here there are no data segments left.
                //SYS_CONSOLE_PRINT("TXe:pkt %x @ %x len %x next @ %x\r\n", pkt, pkt->gpPtr, count, pDrvInst->gpPtrVal);
                pkt->state = DRV_ENCX24J600_TP_WAIT_FOR_GPWRREG;
                abortTask = false;
                break;

            case DRV_ENCX24J600_TP_WAIT_FOR_GPWRREG:
                ptrValue = 0;
                if ((*pDrvInst->busVTable->fpOpResult)(pDrvInst, pkt->operation) == DRV_ENCX24J600_BR_SUCCESS)
                {
                    (void)(*pDrvInst->busVTable->fpPtrRdResult)(pDrvInst, pkt->operation, &ptrValue, DRV_ENCX24J500_TP_OP_READ_GPWRREG);
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
                    abortTask = true;
                }
                break;

            case DRV_ENCX24J600_TP_WAIT_FOR_CTTX:
                abortTask = true;
                break;

            case DRV_ENCX24J600_TP_SET_ETXST:
                reg.value = 0;
                reg.etxst.ETXST = pkt->gpPtr;
                ret = (*pDrvInst->busVTable->fpSfrWr)(pDrvInst, DRV_ENCX24J600_SFR_ETXS, reg, DRV_ENCX24J600_TP_OP_SET_ETXST);
                if (ret == 0U)
                {
                    abortTask = true;
                }
                else
                {
                    pkt->state = DRV_ENCX24J600_TP_SET_ETXLEN;
                }
                break;

            case DRV_ENCX24J600_TP_SET_ETXLEN:
                reg.value = 0;
                count = 0;
                pkt->pDSeg = pkt->pkt->pDSeg;
                while (pkt->pDSeg != NULL)
                {
                    count += pkt->pDSeg->segLen;
                    pkt->pDSeg = pkt->pDSeg->next;
                }
                reg.etxlen.ETXLEN = count;
                ret = (*pDrvInst->busVTable->fpSfrWr)(pDrvInst, DRV_ENCX24J600_SFR_ETXLEN, reg, DRV_ENCX24J600_TP_OP_SET_ETXLEN);
                if (ret == 0U)
                {
                    abortTask = true;
                }
                else
                {
                    //SYS_CONSOLE_PRINT("TX:pkt %x @ %x len %x next @ %x\r\n", pkt, pkt->gpPtr, count, pDrvInst->gpPtrVal);
                    /*if (lastPacketAddr == pkt->gpPtr)
                      {
                      SYS_CONSOLE_MESSAGE("Badd Stuff\r\n");
                      }*/
                    lastPacketAddr = pkt->gpPtr;
                    ret = (*pDrvInst->busVTable->fpReqPktTx)(pDrvInst);
                    if (ret == 0U)
                    {
                        abortTask = true;
                    }
                    else
                    {
                        pkt->state = DRV_ENCX24J600_TP_WAIT_FOR_COMPLETE;
                    }
                }

                break;

            case DRV_ENCX24J600_TP_RQ_PKT_TX:
                ret = (*pDrvInst->busVTable->fpReqPktTx)(pDrvInst);
                if (ret == 0U)
                {
                    abortTask = true;
                }
                else
                {
                    pkt->state = DRV_ENCX24J600_TP_WAIT_FOR_COMPLETE;
                }
                break;

            case DRV_ENCX24J600_TP_WAIT_FOR_COMPLETE:
                abortTask = true;
                break;

            case DRV_ENCX24J600_TP_RST_EIR:
                if(pkt->pkt != NULL)
                {
                    count = 0;
                    pkt->pDSeg = pkt->pkt->pDSeg;
                    while (pkt->pDSeg != NULL)
                    {
                        count += pkt->pDSeg->segLen;
                        pkt->pDSeg = pkt->pDSeg->next;
                    }
                    if (!pDrvInst->mainStateInfo.runningInfo.chkStaInfo.linkState)
                    {
                        pkt->pkt->ackRes = (int8_t)TCPIP_MAC_PKT_ACK_LINK_DOWN;
                    }
                    else
                    {
                        pkt->pkt->ackRes = (int8_t)TCPIP_MAC_PKT_ACK_TX_OK;
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
                if (ret == 0U)
                {
                    abortTask = true;
                }
                else
                {
                    pkt->state = DRV_ENCX24J600_TP_NO_PKT_STATE;
                    pDrvInst->mainStateInfo.runningInfo.ctTx = true;
                    pkt->pkt = NULL;
                    pkt->gpPtr = DRV_ENCX24J600_MEM_SIZE;
                }
                break;

            default:
                // do nothing
                abortTask = true;
                break;
        }
    }

    return 0;
}

int32_t DRV_ENCX24J600_TxPacketEnter(struct S_DRV_ENCX24J600_DriverInfo * pDrvInst, DRV_ENCX24J600_TX_PACKET_INFO *pkt)
{
    pkt->state = DRV_ENCX24J600_TP_NO_PKT_STATE;
    pkt->pkt = NULL;
    pkt->gpPtr = DRV_ENCX24J600_MEM_SIZE;
    return 0;
}

int32_t DRV_ENCX24J600_TxPacketExit(struct S_DRV_ENCX24J600_DriverInfo * pDrvInst, DRV_ENCX24J600_TX_PACKET_INFO *pkt)
{
    return 0;
}


